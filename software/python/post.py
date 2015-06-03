#!/usr/bin/python
'''
see the gspread docs:
http://gspread.readthedocs.org/en/latest/oauth2.html

for info in getting oauth2 setup. 
When you've got the json file downloaded, move it to secrets.json

gdata docs re spreadsheets:
https://gdata-python-client.googlecode.com/hg/pydocs/gdata.spreadsheets.client.html#SpreadsheetsClient-add_record
'''
import json
import argparse
import logging as log
from datetime import datetime
import gdata.spreadsheets.client
import gdata.gauth
import os
from query import get_install_dir, get_user, get_users_file, get_tools_file

# put this in a config
user_spread_key = '1bunn7jx2sxtLwCtbpWVo3ih4prpLRCs_5TdyOyejCfM'

def get_secrets_file():
    return get_install_dir() + "/secrets.json"

def load_secrets():
    with open(get_secrets_file()) as fh:
        secrets = json.load(fh)
    return secrets

def get_tokens():
    secrets = load_secrets()
    token = gdata.gauth.OAuth2Token(
        client_id=secrets["installed"]['client_id'],
        client_secret=secrets["installed"]['client_secret'],
        scope='https://spreadsheets.google.com/feeds/',
        user_agent=__name__)

    log.info(token.generate_authorize_url())
    code = raw_input('What is the verification code? ').strip()
    token.get_access_token(code)
    secrets["refresh_token"] = token.refresh_token
    secrets["access_token"] = token.access_token

    with open(get_secrets_file(),'w') as fh:
        json.dump(secrets, fh)
        log.info("wrote refresh and access token to secrets.json")

def get_client():
    secrets = load_secrets()
    token = gdata.gauth.OAuth2Token(
        client_id=secrets["installed"]['client_id'],
        client_secret=secrets["installed"]['client_secret'],
        scope='https://spreadsheets.google.com/feeds/',
        user_agent=__name__,
        access_token=secrets['access_token'],
        refresh_token=secrets['refresh_token'])

    client = gdata.spreadsheets.client.SpreadsheetsClient()
    token.authorize(client)
   
    return client

def worksheet_dict(feed):
    d = {}
    for i, entry in enumerate(feed.entry):
        d[entry.title.text] = entry.id.text.split('/')[-1] 
    return d

def fetch_tools():
    log.debug("fetching tools")
    client = get_client()
    sheets = worksheet_dict(client.get_worksheets(user_spread_key))
    worksheet_id = sheets['tools']
    all_tools = []
    for entry in client.get_list_feed(user_spread_key, worksheet_id).entry:
        all_tools.append(entry.to_dict())
    log.info("fetched %d tools" % len(all_tools))
    with open(get_tools_file(), 'w') as fh:
        json.dump(all_tools, fh)

def fetch_users():
    log.debug("fetching users")
    client = get_client()
    sheets = worksheet_dict(client.get_worksheets(user_spread_key))
    worksheet_id = sheets['users']

    all_users = []
    for entry in client.get_list_feed(user_spread_key, worksheet_id).entry:
        all_users.append(entry.to_dict())

    log.info("fetched %d users" % len(all_users))
    with open(get_users_file(), 'w') as fh:
        json.dump(all_users, fh)

def log_unknown_rfid(rfid):
    log.debug("logging unknown rfid [%s]" % rfid)
    client = get_client()
    sheets = worksheet_dict(client.get_worksheets(user_spread_key))
    worksheet_id = sheets['unknown']
    entry = gdata.spreadsheets.data.ListEntry()
    entry.set_value('rfid', rfid)
    entry.set_value('date', str(datetime.now()))
    client.add_list_entry(entry, user_spread_key, worksheet_id)

def log_time(tool, time, name, rfid):
    log.debug("logging %s on %s for user %s [%s]" % (time, tool, name, rfid))
    client = get_client()
    sheets = worksheet_dict(client.get_worksheets(user_spread_key))
    try:
        worksheet_id = sheets[tool]
    except KeyError:
        log.error("no such tool in spreadsheet")
        exit(1)

    entry = gdata.spreadsheets.data.ListEntry()
    entry.set_value('rfid', rfid)
    entry.set_value('name', name)
    entry.set_value('date', str(datetime.now()))
    entry.set_value('time', time)
    client.add_list_entry(entry, user_spread_key, worksheet_id)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="yun spreadsheet interface")
    parser.add_argument('--auth-token', action='store_const', const=True,
        help="do oauth2 token stuff")
    parser.add_argument('--update-cache', action='store_const', const=True,
        help="refresh the user and tool cache")
    parser.add_argument('--rfid', action='store',
        help="rfid")
    parser.add_argument('--time', action='store',
        help="specify time to log")
    parser.add_argument('--log-tool', action='store',
        help="log a tool usage")
    parser.add_argument('--log-unknown-rfid', action='store_const', const=True,
        help="log an unknown user")

    # setup logging
    log.basicConfig(level=log.DEBUG,
                format='%(asctime)s %(levelname)-8s %(message)s',
                datefmt='%m-%d %H:%M:S',
                filename=get_install_dir() + '/post.log',
                filemode='a')

    args = parser.parse_args()

    # do everything in a try so we can log exceptions
    try:
        if args.auth_token:
            get_tokens()

        elif args.update_cache:
            fetch_users()
            fetch_tools()

        elif args.log_tool:
            if not (args.time and args.rfid):
                parser.error("--log-tool requires --time and --rfid")
            name, tools = get_user(args.rfid)
            log_time(args.log_tool, args.time, name, args.rfid)
        
        elif args.log_unknown_rfid:
            log_unknown_rfid(args.rfid)

    except Exception as e:
    	if 'Name or service not known' in str(e):
            log.error("no internet connection")
            exit(1)
        elif 'Server responded with: 502' in str(e):
            log.error("temporary google service failure")
            exit(1)
        else:
            log.exception(e)
            exit(1)


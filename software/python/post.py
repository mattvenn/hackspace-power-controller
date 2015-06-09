#!/usr/bin/env python
'''
read setup.md for info in getting oauth2 setup. 

gspread docs:
http://gspread.readthedocs.org/en/latest/oauth2.html
https://gdata-python-client.googlecode.com/hg/pydocs/gdata.spreadsheets.client.html#SpreadsheetsClient-add_record
'''
import json
import argparse
import logging
from logging.handlers import RotatingFileHandler
from datetime import datetime
import gdata.spreadsheets.client
import gdata.gauth
import fcntl
import socket
from query import get_install_dir, get_user, get_users_file, get_tools_file

log = logging.getLogger('')


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
    code = raw_input('click the above link, then paste the code here: ').strip()
    token.get_access_token(code)
    secrets["refresh_token"] = token.refresh_token
    secrets["access_token"] = token.access_token
    secrets['spread_key'] = args.spread_key

    with open(get_secrets_file(),'w') as fh:
        json.dump(secrets, fh)
        log.info("wrote refresh and access token to secrets.json")

def get_client():
    secrets = load_secrets()
    try:
        token = gdata.gauth.OAuth2Token(
            client_id=secrets["installed"]['client_id'],
            client_secret=secrets["installed"]['client_secret'],
            scope='https://spreadsheets.google.com/feeds/',
            user_agent=__name__,
                access_token=secrets['access_token'],
                refresh_token=secrets['refresh_token'])
    except KeyError as e:
        log.error("you must do oauth2 setup first - read the setup.md file")
        exit(1)

    client = gdata.spreadsheets.client.SpreadsheetsClient()
    token.authorize(client)
   
    return client, secrets['spread_key']

def worksheet_dict(feed):
    d = {}
    for i, entry in enumerate(feed.entry):
        d[entry.title.text] = entry.id.text.split('/')[-1] 
    return d

def fetch_tools():
    log.debug("fetching tools")
    client, spread_key = get_client()
    sheets = worksheet_dict(client.get_worksheets(spread_key))
    worksheet_id = sheets['tools']
    all_tools = []
    for entry in client.get_list_feed(spread_key, worksheet_id).entry:
        all_tools.append(entry.to_dict())
    log.info("fetched %d tools" % len(all_tools))
    with open(get_tools_file(), 'w') as fh:
        json.dump(all_tools, fh)

def fetch_users():
    log.debug("fetching users")
    client, spread_key = get_client()
    sheets = worksheet_dict(client.get_worksheets(spread_key))
    worksheet_id = sheets['users']

    all_users = []
    for entry in client.get_list_feed(spread_key, worksheet_id).entry:
        all_users.append(entry.to_dict())

    log.info("fetched %d users" % len(all_users))
    with open(get_users_file(), 'w') as fh:
        json.dump(all_users, fh)

def log_unknown_rfid(rfid):
    log.info("logging unknown rfid [%s]" % rfid)
    client, spread_key = get_client()
    sheets = worksheet_dict(client.get_worksheets(spread_key))
    worksheet_id = sheets['unknown']
    entry = gdata.spreadsheets.data.ListEntry()
    entry.set_value('rfid', rfid)
    entry.set_value('date', str(datetime.now()))
    client.add_list_entry(entry, spread_key, worksheet_id)

def log_time(tool, time, name, rfid):
    log.info("logging %s on %s for user %s [%s]" % (time, tool, name, rfid))
    client, spread_key = get_client()
    sheets = worksheet_dict(client.get_worksheets(spread_key))
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
    client.add_list_entry(entry, spread_key, worksheet_id)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="yun spreadsheet interface")
    parser.add_argument('--auth-token', action='store_const', const=True,
        help="do oauth2 token stuff")
    parser.add_argument('--spread-key', action='store',
        help="store the spreadsheet key")
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
    log.setLevel(logging.DEBUG)
    log_format = logging.Formatter("%(asctime)s - %(levelname)-8s - %(message)s")

    ch = logging.StreamHandler()
    ch.setFormatter(log_format)
    log.addHandler(ch)

    fh = logging.handlers.RotatingFileHandler(get_install_dir() + '/post.log', maxBytes=(1024*1024), backupCount=7)
    fh.setFormatter(log_format)
    log.addHandler(fh)

    args = parser.parse_args()

    # as this program gets run separately, if there is a network error 
    # lots can end up running, killing the Yun by eating memory
    file = "/tmp/post.lock"
    fd = open(file, 'w')
    try:
        fcntl.lockf(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
        log.debug("checked lock ok")
    except IOError:
        log.debug("another process is running with lock. quitting!")
        exit(1)

    # do everything in a try so we can log exceptions
    try:
        if args.auth_token:
            if not args.spread_key:
                parser.error("--auth-token requires --spread-key")
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

        log.debug("ending")
        exit(0)

    except socket.gaierror as e:
        log.error("temporary google service failure")
    except socket.error as e:
        log.error("socket err: %s" % e)
    except Exception as e:
        log.exception(e)

    exit(1)

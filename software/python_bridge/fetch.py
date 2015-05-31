#!/usr/bin/python
'''
see the gspread docs:
http://gspread.readthedocs.org/en/latest/oauth2.html

for info in getting oauth2 setup. 
When you've got the json file downloaded, move it to secrets.json
'''
import json
import argparse

import gdata.spreadsheets.client
import gdata.gauth
import os

def get_secrets_file():
    return os.path.dirname(os.path.realpath(__file__)) + "/secrets.json"

def get_users_file():
    return os.path.dirname(os.path.realpath(__file__)) + "/users.json"

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

    print(token.generate_authorize_url())
    code = raw_input('What is the verification code? ').strip()
    token.get_access_token(code)
    secrets["refresh_token"] = token.refresh_token
    secrets["access_token"] = token.access_token

    with open(get_secrets_file(),'w') as fh:
        json.dump(secrets, fh)
        print("wrote refresh and access token to secrets.json")

def get_sheet(sheet_key):

    secrets = load_secrets()

    token = gdata.gauth.OAuth2Token(
        client_id=secrets["installed"]['client_id'],
        client_secret=secrets["installed"]['client_secret'],
        scope='https://spreadsheets.google.com/feeds/',
        user_agent=__name__,
        access_token=secrets['access_token'],
        refresh_token=secrets['refresh_token'])

    spr_client = gdata.spreadsheets.client.SpreadsheetsClient()
    token.authorize(spr_client)
   
    return spr_client

def fetch_users(spr_client):
    all_users = []
    for entry in spr_client.get_list_feed(sheet_key, 'od6').entry:
        all_users.append(entry.to_dict())
    print("fetched %d users" % len(all_users))
    with open(get_users_file(), 'w') as fh:
        json.dump(all_users, fh)

def get_user(rfid):
    with open(get_users_file()) as fh:
        all_users = json.load(fh)

    for user, id in zip(all_users, range(len(all_users))):
        if user['rfid'] == rfid:
            return id, user['name']
    exit(-1)

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="yun spreadsheet interface")
    parser.add_argument('--auth-token', action='store_const', const=True,
        help="do oauth2 token stuff")
    parser.add_argument('--fetch-users', action='store_const', const=True,
        help="fetch users")
    parser.add_argument('--check-rfid', action='store',
        help="check rfid")
    args = parser.parse_args()

    if args.auth_token:
        get_tokens()
    if args.fetch_users:
        sheet_key = '1bunn7jx2sxtLwCtbpWVo3ih4prpLRCs_5TdyOyejCfM'
        sheet = get_sheet(sheet_key)
        fetch_users(sheet)
    if args.check_rfid:
        id, name = get_user(args.check_rfid)
        print("%d %s" % (id, name))

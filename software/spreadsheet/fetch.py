import gspread
import json
import argparse
from oauth2client.client import SignedJwtAssertionCredentials

def get_sheet(name):
    json_key = json.load(open('secrets.json'))
    scope = ['https://spreadsheets.google.com/feeds']
    credentials = SignedJwtAssertionCredentials(json_key['client_email'], json_key['private_key'], scope)
    gc = gspread.authorize(credentials)

    # Open a worksheet from spreadsheet with one shot
    sheet = gc.open(name)
    return sheet

def fetch_users(sheet):
    users = sheet.worksheet('users')
    all_users = users.get_all_records()
    print("fetched %d users" % len(all_users))
    with open('users.json', 'w') as fh:
        json.dump(all_users, fh)

def get_user(rfid):
    with open('users.json') as fh:
        all_users = json.load(fh)

    for user in all_users:
        if user['rfid'] == rfid:
            return user['name']

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="yun spreadsheet interfac")
    parser.add_argument('--fetch-users', action='store_const', const=True,
        help="fetch users")
    parser.add_argument('--check-rfid', action='store',
        help="check rfid")
    args = parser.parse_args()


    if args.fetch_users:
        sheet = get_sheet("hackspace-power-controller")
        fetch_users(sheet)
    if args.check_rfid:
        name = get_user(args.check_rfid)
        print(name)

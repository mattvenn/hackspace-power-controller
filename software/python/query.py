#!/usr/bin/python
import json
import subprocess
import argparse
import logging as log
from datetime import datetime
import os

def get_install_dir():
    return os.path.dirname(os.path.realpath(__file__))

def get_post_prog():
    return get_install_dir() + "/post.py"

def get_users_file():
    return get_install_dir() + "/users.json"

def get_tools_file():
    return get_install_dir() + "/tools.json"

def get_tools():
    with open(get_tools_file()) as fh:
        all_tools = json.load(fh)
        return all_tools

def get_user(rfid):
    with open(get_users_file()) as fh:
        all_users = json.load(fh)

    for user in all_users:
        if user['rfid'] == rfid:
            log.debug("found user %s for rfid [%s]" % (user['name'], rfid))
            return user['name'], user['tools']

    raise KeyError("no user")

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="yun spreadsheet interface")
    parser.add_argument('--check-user', action='store_const', const=True,
        help="check an rfid")
    parser.add_argument('--list-tools', action='store_const', const=True,
        help="list tools for arduino to read")
    parser.add_argument('--rfid', action='store',
        help="rfid")
    parser.add_argument('--time', action='store',
        help="specify time to log")
    parser.add_argument('--log-tool', action='store',
        help="log a tool usage")

    # setup logging
    log.basicConfig(level=log.DEBUG,
                format='%(asctime)s %(levelname)-8s %(message)s',
                datefmt='%m-%d %H:%M:%S',
                filename=get_install_dir() + '/query.log',
                filemode='a')

    args = parser.parse_args()

    # do everything in a try so we can log exceptions
    try:
        if args.list_tools:
            for tool in get_tools():
                log.info("listing tools")
                print("%s,%s,%s" % (tool['name'], tool['id'], tool['operational']))
        elif args.log_tool:
            if not (args.time and args.rfid):
                parser.error("--log-tool requires --time and --rfid")

            # separate process so we can return immediately
            name, tools = get_user(args.rfid)
            log.info("logging tool %s used for %s by [%s]" % 
                (args.log_tool, args.time, args.rfid))
            subprocess.Popen([get_post_prog(),
                "--log-tool", args.log_tool,
                "--rfid", args.rfid, "--time", args.time])

        elif args.check_user:
            try:
                log.info("checking [%s]" % args.rfid)
                name, tools = get_user(args.rfid)
                print("%s,%s" % (name, tools))
            except KeyError as e:
                # separate process so we can return immediately
                log.warning("logging unknown rfid [%s]" % args.rfid)
                subprocess.Popen([get_post_prog(), "--log-unknown-rfid", "--rfid", args.rfid])
                # exit 1 to let Arduino know it failed
                exit(1)

    # not expecting anything, so catch and log it with a stacktrace
    except Exception as e:
        log.exception(e)
        exit(1)

    log.info("ending")


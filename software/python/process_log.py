#!/usr/bin/python
from datetime import datetime
import argparse


def process():
    limit_lines = 0
    line_count = 0
    last_dt = None
    last_line = ""
    repeats = 0
    with open(args.log) as fh:
        for line in fh.readlines():
            line_count += 1
            try:
                line_d = ' '.join(line.split(' ')[0:2])
                line_t = ' '.join(line.split(' ')[2:])
                dt = datetime.strptime(line_d, "%m-%d %H:%M:%S")
            except ValueError:
                next
            if last_dt is not None:
                delta = dt - last_dt
                if delta.total_seconds() > args.time:
                    #print delta.seconds
                    print("from: %-30s" % last_line.strip())
                    print('to  : %-30s took %s secs' % (line.strip(), delta.total_seconds()))
                    limit_lines += 1
                if line_t == repeated_line:
                    repeats += 1
            else:
                repeated_line = line_t
            last_dt = dt
            last_line = line

    print("%d [%d%%] lines [of %d] longer than %d" % (limit_lines, (limit_lines/float(line_count))*100,line_count, args.time))
    print("%d cycles" % (repeats))


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="log processor")
    parser.add_argument('--time', action='store', type=int, default=5,
        help="time limit between lines")
    parser.add_argument('--log', action='store', default='serial.log',
        help="log file to process")

    args = parser.parse_args()
    process()

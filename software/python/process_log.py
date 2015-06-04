#!/usr/bin/python
from datetime import datetime
log_file = 'serial.log'

limit = 5
last_dt = None
line_count = 0
limit_lines = 0
last_line = ""
with open(log_file) as fh:
    for line in fh.readlines():
        line_count += 1
        try:
            dline = line.split(' ')[0:2]
            dline = ' '.join(dline)
            dt = datetime.strptime(dline, "%m-%d %H:%M:%S")
        except ValueError:
            next
        if last_dt is not None:
            delta = dt - last_dt
            if delta.total_seconds() > limit:
                #print delta.seconds
                print('>' + last_line.strip())
                print('<' + line.strip() + ' ' + str(delta.total_seconds()))
                limit_lines += 1
        last_dt = dt
        last_line = line

print("%d lines [of %d] longer than %d" % (limit_lines, line_count, limit))

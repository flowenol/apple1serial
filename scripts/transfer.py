import sys
import serial
import select
import signal
import time


def print_usage():
    print('Usage: python serialfile.py -in <serial device> > file', file=sys.stderr)
    print('Usage: python serialfile.py -out <serial device> < file', file=sys.stderr)


def get_serial(port: str) -> serial.Serial:
    s = serial.Serial()
    s.baudrate = 9600
    s.parity = serial.PARITY_NONE
    s.stopbits = serial.STOPBITS_ONE
    s.bytesize = serial.EIGHTBITS
    s.port = port
    s.timeout = None
    s.write_timeout = None
    return s

def data_ready_on_stdin():
    return bool(select.select([sys.stdin], [], [], 0)[0])

"""
This utility receives/sends files via selected serial port to the
Apple-1 Serial Interface card.

This utility requires pyserial module to be installed:

pip3 install pyserial
"""
if __name__ == "__main__":

    if len(sys.argv) != 3:
        print_usage()
        exit(1)

    if sys.argv[1] == '-in':
        print('Press Ctrl+C after the transmission ends', file=sys.stderr)
        
        # terminate silently on SIGINT
        signal.signal(signal.SIGINT, lambda s, fr: exit(0))
        with get_serial(sys.argv[2]) as s:
            while True:
                sys.stdout.buffer.write(s.read())

    elif sys.argv[1] == '-out':
        # check if the data is available on stdin
        if not data_ready_on_stdin():
            exit(0)
        with get_serial(sys.argv[2]) as s:
            data = sys.stdin.buffer.read()
            while len(data) > 0:
                s.write(data)
                data = sys.stdin.buffer.read()
    else:
        print(f'Unknown option: {sys.argv[1]}', file=sys.stderr)
        print_usage()
        exit(1)

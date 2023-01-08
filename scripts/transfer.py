import sys
import serial
import select
import signal
import time

bytes_processed: int  = 0

def print_usage_and_exit():
    print('Usage: python serialfile.py -in <serial device> [ > file ]', file=sys.stderr)
    print('Usage: python serialfile.py -out <serial device> < file', file=sys.stderr)
    exit(1)


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

def data_ready_on_port(port: serial.Serial):
    return bool(select.select([port.fileno()], [], [], 0)[0])


def handle_inbound_transmission(port: str):
    global bytes_processed

    with get_serial(port) as s:
        print('Waiting for data... ', file=sys.stderr, end='', flush=True)
        while not data_ready_on_port(s):
            pass
        print('OK', file=sys.stderr)

        s.timeout = 1
        while True:
            data = s.read()
            if len(data) > 0:
                sys.stdout.buffer.write(data)
                bytes_processed += len(data)
            else:
                break
        print('Transmission finished', file=sys.stderr)


def handle_outbound_transmission(port: str):
    global bytes_processed

    if not data_ready_on_stdin():
        print_usage_and_exit()

    with get_serial(port) as s:
        data = sys.stdin.buffer.read()
        while len(data) > 0:
            s.write(data)
            bytes_processed += len(data)
            data = sys.stdin.buffer.read()


"""
This utility receives/sends files via selected serial port to the
Apple-1 Serial Interface card.

This utility requires pyserial module to be installed:

pip3 install pyserial
"""
if __name__ == "__main__":

    if len(sys.argv) != 3:
        print_usage_and_exit()

    def print_bytes_processed_and_exit(status: int):
        print(f'Total bytes processed: {bytes_processed}', file=sys.stderr)
        exit(status)

    # terminate silently on SIGINT
    signal.signal(signal.SIGINT, lambda s, fr: print_bytes_processed_and_exit(1))

    if sys.argv[1] == '-in':
        handle_inbound_transmission(sys.argv[2])

    elif sys.argv[1] == '-out':
        handle_outbound_transmission(sys.argv[2])

    else:
        print(f'Unknown option: {sys.argv[1]}', file=sys.stderr)
        print_usage_and_exit()

    print_bytes_processed_and_exit(0)

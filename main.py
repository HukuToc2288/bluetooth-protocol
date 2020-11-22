# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

import crc8


class Packet:
    def __init__(self):
        pass


def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm')


# See PyCharm help at https://www.jetbrains.com/help/pycharm/


def send_byte(byte):
    # stub
    pass


def receive_byte():
    # stub
    return 0


def make_packet(packet_type, data):
    p = Packet
    p.packetType = packet_type
    p.length = len(data)
    p.payload = data
    p.crc8 = crc8.crc8().update(data)


def send_packet(p):
    send_byte(p.packetType)
    send_byte(p.length / 256)
    send_byte(p.length % 256)
    for i in p.payload:
        send_byte(i)
    send_byte(p.crc8)


def check_crc(p):
    crc = crc8.crc8()
    crc.update(p.packetType)
    crc.update(p.length / 256)
    crc.update(p.length % 256)
    for i in range(0, p.length):
        crc.update(p.payload[i])
    return int(crc) == p.crc8


def try_receive_packet():
    p = Packet
    p.packetType = receive_byte()
    p.length = receive_byte() * 256 + receive_byte()
    p.payload = [0] * p.length
    for i in range(0, p.length):
        p.payload[i] = receive_byte()
    p.crc8 = receive_byte()
    if check_crc(p):
        return 0
    else:
        return 1

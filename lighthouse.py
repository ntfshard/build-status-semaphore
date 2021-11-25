#!python3
# lighthouse.py

import sys

import serial
import requests

import configlighthouse

# possible commands for device(one letter command):
# Capital -- enable, regular -- disable; AaRrYyGgBbWw: All, Red Yellow Green Blue White
def send2device(s):
    serialPort = serial.Serial(port=configlighthouse.comport, baudrate=9600)
    serialPort.reset_input_buffer()
    serialPort.write(s.encode())
    print("send2device(" + configlighthouse.comport + "): " + s)

def getExtraArgs():
    if len(sys.argv) != 3:
        return ""
    else:
        return sys.argv[2]


if __name__ == "__main__":
    if len(sys.argv) != 3 and len(sys.argv) != 2:
        print("Usage: lighthouse.py link-to-dir [extraArg]\n    link-to-dir - https://localhost/job/something/job/branch\n    extraArg - (W|w)(B|b)\n    W-white on, w-white off, B-blue on, b-blue off")
        exit(1)

    send2device('a') # all off

    r = requests.get(sys.argv[1] + '/api/json', auth=(configlighthouse.user, configlighthouse.password), verify=False)
    if not r.ok:
        send2device("RG" + getExtraArgs())
        print("Error: " + str(r.status_code) + " " + r.reason)
        exit(2);

    good_counter = 0
    for j in r.json()['jobs']:
        if j['_class'] ==  "org.jenkinsci.plugins.workflow.job.WorkflowJob":
            print("Job: " + j['name'] + " is " + j["color"])
            if j["color"] == "red":
                send2device("R" + getExtraArgs())
                exit(0)
            if j["color"] == "blue":
                good_counter += 1

    if good_counter > 0:
        send2device("G" + getExtraArgs())
    else:
        send2device("Y" + getExtraArgs())

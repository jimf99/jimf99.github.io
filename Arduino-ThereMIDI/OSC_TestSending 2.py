import time
from pythonosc import udp_client

s = udp_client.SimpleUDPClient('127.0.0.1', 4560)

lowr = 40
highr=lowr+12*2
dly1=0.3

while True :
    for i in range(0,highr-lowr):
        note1=lowr+i
        s.send_message("/trigger/prophet", [note1, 60,0.0])        
        time.sleep(dly1)
    time.sleep(1)
    for i in range(0,highr-lowr):
        note1=highr-i
        s.send_message("/trigger/prophet", [note1, 60,0.0])
        time.sleep(dly1)
    time.sleep(1)

# live_loop :foo do
#	use_real_time
#	a, b, c = sync "/osc*/trigger/prophet"
# 	synth :prophet, note: a, cutoff: b, sustain: c
#	end




# Sonic Pi Theremin Receiver
# This code listens for OSC messages from the Python GUI.

play 69
sleep(1)
play 70
sleep(1)
play 71
sleep(3)

live_loop :theremin_receiver do
  # get OSC message with the address "/theremin/control".
  osc_data = sync "/osc*/theremin/control"
  val1 = osc_data[0]
  val2 = osc_data[1]/100
  n=val1
  a=val2
  use_synth :piano
  play note: n, amp: a, attack: 0.1, decay: 0.1, sustain: 0.2
end

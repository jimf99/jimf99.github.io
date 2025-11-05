# EPIC! Sonic Pi OSC Listener

use_osc "localhost", 4560  # Make sure the port matches the one used in your Tkinter script

# Define a function to handle incoming OSC messages
define :play_note do |key|
  case key
  when "1" then play :C4, release: 0.5
  when "2" then play :D4, release: 0.5
  when "3" then play :E4, release: 0.5
  when "4" then play :F4, release: 0.5
  when "5" then play :G4, release: 0.5
  when "6" then play :A4, release: 0.5
  when "7" then play :B4, release: 0.5
  when "8" then play :C5, release: 0.5
  when "9" then play :D5, release: 0.5
  when "0" then play :E5, release: 0.5
  when "A" then play :F5, release: 0.5  # Additional Notes
  when "B" then play :G5, release: 0.5
  when "C" then play :A5, release: 0.5
  when "D" then play :B5, release: 0.5
  when "E" then play :C6, release: 0.5
  when "F" then play :D6, release: 0.5
  when "G" then play :E6, release: 0.5
  when "H" then play :F6, release: 0.5
  when "I" then play :G6, release: 0.5
  when "J" then play :A6, release: 0.5
  when "K" then play :B6, release: 0.5
  when "L" then play :C7, release: 0.5
  when "M" then play :D7, release: 0.5
  when "N" then play :E7, release: 0.5
  when "O" then play :F7, release: 0.5
  when "P" then play :G7, release: 0.5
  when "Q" then play :A7, release: 0.5
  when "R" then play :B7, release: 0.5
  when "S" then play :C8, release: 0.5
  else
    puts "Unknown key: #{key}"
  end
end

# Listen for OSC messages and respond accordingly
osc "/key" do |args|
  key_received = args[0].to_s
  play_note key_received
end

# Keep the script running
live_loop :keep_alive do
  sleep 1
end

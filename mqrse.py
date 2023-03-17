'''
Same thing mqrse.ino does but on a computer. Working well enough.

Using pygame for sound: https://stackoverflow.com/a/63980631
'''

import logging
import pygame
import numpy as np
import paho.mqtt.client as mqtt


logging.basicConfig(level=logging.INFO, force=True, format='%(asctime)s %(levelname)s %(message)s')

'''
https://morsecode.world/international/timing.html
Dit: 1 unit
Dah: 3 units
Intra - character space(the gap between dits and dahs within a character): 1 unit
Inter - character space(the gap between the characters of a word): 3 units
Word space(the gap between two words): 7 units
'''
UNIT = 0.1  # seconds
DEFAULT_FREQUENCY = 440  # Hz

LETTER_TO_MORSE = {
    'A': '.-',
    'B': '-...',
    'C': '-.-.',
    'D': '-..',
    'E': '.',
    'F': '..-.',
    'G': '--.',
    'H': '....',
    'I': '..',
    'J': '.---',
    'K': '-.-',
    'L': '.-..',
    'M': '--',
    'N': '-.',
    'O': '---',
    'P': '.--.',
    'Q': '--.-',
    'R': '.-.',
    'S': '...',
    'T': '-',
    'U': '..-',
    'V': '...-',
    'W': '.--',
    'X': '-..-',
    'Y': '-.--',
    'Z': '--..',
    '?': '..--..',
}

RATE = 44100  # Samples per second. PyGame's default.


def make_buffer(duration, frequency=DEFAULT_FREQUENCY, volume=1):
    # Is this correct for square wave? Then I could remove numpy dependency.
    # If you're not lazy, sine is also possible without numpy.
    one_cycle =  [volume] * round(RATE / (4 * frequency)) +\
                [-volume] * round(RATE / (4 * frequency))
    waveform = one_cycle * round(duration * frequency)
    return waveform
    # return volume * np.sin(2 * np.pi * np.arange(RATE * duration) * frequency / RATE).astype(np.float32)


DIT = make_buffer(duration=UNIT)
DAH = make_buffer(duration=UNIT * 3)
INTRA = make_buffer(duration=UNIT, volume=0)
INTER = make_buffer(duration=UNIT * 3, volume=0)

# Pre-compute the buffers for each letter.
LETTER_TO_BUFFER = {}
for letter, code in LETTER_TO_MORSE.items():
    buf = []
    for char in code:
        if char == '.':
            buf.append(DIT)
        elif char == '-':
            buf.append(DAH)
        buf.append(INTRA)
    buf.pop()  # Get rid of last intra-character space.
    buf.append(INTER)
    LETTER_TO_BUFFER[letter] = np.concatenate(buf)



def play_morse(text):
    text = text.upper()
    buffer = []
    for char in text:
        if char == ' ':
            buffer.append(INTER)
        else:
            if char not in LETTER_TO_MORSE:
                logging.info('Unknown character: {}'.format(char))
                char = '?'
            buffer.append(LETTER_TO_BUFFER[char])
    buffer = np.concatenate(buffer)
    sound = pygame.mixer.Sound(buffer)
    sound.play(0)
    snd_len = sound.get_length()
    logging.info(f'{snd_len:5.1f}s → "{text}"')
    pygame.time.wait(int(snd_len * 1000))


pygame.mixer.pre_init(frequency=RATE) # Not so important. Default rate is 44100 anyway.
pygame.mixer.init(size=32)

client = mqtt.Client()
client.connect("localhost", 1883, 60)

def on_connect(client, userdata, flags, rc):
    logging.info("Connected with result code " + str(rc))
    client.subscribe("morse")

def on_message(client, userdata, msg):
    text = msg.payload.decode()
    if text:
        play_morse(text)
        play_morse('  ') # Some pause between MQTT messages.

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()

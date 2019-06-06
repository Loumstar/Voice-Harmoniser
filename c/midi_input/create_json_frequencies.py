import re, json
"""
with open('./piano_frequencies.txt') as f:
    frequency_list = f.readlines()

json_list = []

for f in frequency_list:
    res = re.findall('([\w#]+)/?([\w#]*)[\s]+([\d.]+)', f)
    json_list.append({
        'frequency': res[0][2],
        'notes': res[0][0:2] if res[0][1] != '' else [res[0][0]]
    })

with open('./piano_frequencies.json', 'w+') as g:
    g.write(json.dumps(json_list, indent=4, sort_keys=True))

"""

with open('./piano_frequencies.json', 'r') as f:
    frequencies_json = json.loads(f.read())

for i, freq in enumerate(frequencies_json):
    frequencies_json[i]['number'] = len(frequencies_json) + 3 - i
    if i != 0 and i != len(frequencies_json) - 1:
        frequencies_json[i]['bounds'] = {
            'lb': round(float(freq['frequency']) + (0.5 * (float(frequencies_json[i+1]['frequency']) - float(freq['frequency']))), 6),
            'ub': round(float(freq['frequency']) + (0.5 * (float(frequencies_json[i-1]['frequency']) - float(freq['frequency']))), 6)
        }
    elif i == 0:
        frequencies_json[i]['bounds'] = {
                    'lb': round(float(freq['frequency']) + (0.5 * (float(frequencies_json[i+1]['frequency']) - float(freq['frequency']))), 6),
        }
    else:
        frequencies_json[i]['bounds'] = {
            'ub': round(float(freq['frequency']) + (0.5 * (float(frequencies_json[i-1]['frequency']) - float(freq['frequency']))), 6)
        }

with open('./piano_frequencies.json', 'w') as g:
    g.write(json.dumps(frequencies_json, indent=4, sort_keys=True))

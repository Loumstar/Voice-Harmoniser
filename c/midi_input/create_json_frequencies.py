import re, json

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
import json

dic_device = {
    "801002":"3c65ba4a411b4d6197ee46fd3e8d045d",
    "801003":"7e4e5ff11f014041a8975399ad08c02b",
    "801004":"9c0b27c9e54b4db5aea08d889e38c346"
}

f = file("devicd_db.json","w")
json.dump(dic_device, f)
f.close()

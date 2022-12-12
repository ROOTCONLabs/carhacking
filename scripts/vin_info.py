import requests

def get_vinfo(vin):
    url = 'https://www.nhtsa.gov/webapi/api/Recalls/vehicle/modelyear/'
    url += vin[9:13]
    url += '/make/'
    url += vin[0:3]
    url += '/model/'
    url += vin[3:8]
    response = requests.get(url)
    if response.status_code != 200:
        print('Failed to get vehicle info')
        return

    print(response.json())

vin = input('Enter a VIN: ')
get_vinfo(vin)

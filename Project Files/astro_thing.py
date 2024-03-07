import requests
from flask import Flask, request, jsonify
#import serial
from astropy.coordinates import EarthLocation, AltAz, SkyCoord, get_body
from astropy.time import Time
import astropy.units as u

app = Flask(__name__)


def get_location():
    try:
        response = requests.get('https://ipinfo.io')
        data = response.json()
        location = data['loc'].split(',')
        return float(location[0]), float(location[1])  # return as tuple of floats  
    except Exception as e:
        print("No location found:", e)
        return None
    
#celestial objects to track
celestial_objects = ['Mercury', 'Venus', 'Mars', 'Jupiter', 'Saturn', 'Uranus', 'Neptune', 'Sun', 'Moon']

# Get your location
location = get_location()
print("Location:", location)
print("Observe time:", Time.now())
if location is None:
    print("Could not get location")
    exit()

# Define your location
earth_location = EarthLocation(lat=location[0]*u.deg, lon=location[1]*u.deg, height=0*u.m)

@app.route('/calculate', methods=['POST'])
def calculate():
    index = int(request.json['index'])
    selected_object = celestial_objects[index]

    # Get your location
    response = requests.get('https://ipinfo.io')
    data = response.json()
    location = data['loc'].split(',')
    lat, lon = float(location[0]), float(location[1])

    now = Time.now()

    # Create an EarthLocation object for your location
    earth_location = EarthLocation(lat=lat*u.deg, lon=lon*u.deg)

    # Create a SkyCoord object for the selected celestial object
    obj = SkyCoord.from_name(selected_object)

    # Transform the SkyCoord object to AltAz
    obj_altaz = obj.transform_to(AltAz(obstime=now, location=earth_location))

    # Convert the azimuth and altitude to integers
    obj_az = int(obj_altaz.az.deg)
    obj_alt = int(obj_altaz.alt.deg)

    return jsonify({'azimuth': obj_az, 'altitude': obj_alt})

@app.route('/calculate', methods=['GET'])
def calculate_get():
    # Get the current time
    now = Time.now()

    # Create a list to store the data for each celestial object
    data = []

    # Calculate the altitude and azimuth for each celestial object
    for obj_name in celestial_objects:
        obj = get_body(obj_name, now)
        obj_altaz = obj.transform_to(AltAz(obstime=now, location=earth_location))
        data.append(f"Name: {obj_name}, Altitude: {obj_altaz.alt.deg}, Azimuth: {obj_altaz.az.deg}")

    # Create an HTML string with CSS for the background color and text color
    html = '<html><body style="background-color: black; color: white;">'

    # Add the data to the HTML string with <br> tags
    html += '<br>'.join(data)

    # Close the HTML tags
    html += '</body></html>'

    # Return the HTML string
    return html

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
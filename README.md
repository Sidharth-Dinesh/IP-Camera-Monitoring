# IP-Camera-Monitoring

1. Popular IP camera vendors in India from Indiamart and CPPlus website.
2. Popular IP camera vendors with protocols mentioned were researched and their datasheets collected.
3. Captured the streams from the IP cameras using video component.
4. Sending the frames captured via Video component to Object Detection.
5. Detect Objects in the frame using Shunya ai API.
6. Detect Persons in the frame using Shunya ai API.
7. Calculated the time of the detection based on the fps of the stream.
8. Stored the detected objects in the Influxdb database using Shunya Interfaces API.

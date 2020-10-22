#Enabling Wifi on Ubuntu Server (Raspberry pis)
Much of my process involved trial and error. For Ubuntu Server 20.04 LTS this is what I found to be effective.

1. edit `/etc/netplan/50-cloud-init.yaml`
2. add the following at the bottom
    ```
    wifis:
            YOUR_INTERFACE_NAME:
                optional: true
                access-points:
                    "YOUR_WIFI_SSID":
                        password: "YOUR_WIFI_PASSWORD"
                dhcp4: true
    ```
3. run `sudo netplan --debug test`
4. run `sudo netplan --debug generate`
5. run `sudo netplan --debug apply`
    This will test your netplan, generate it, and apply it to your device.
6. Restart your pi (`sudo shutdown -r now`)
You will run into issues...


The other option... is to not be stubborn as hell, and just download raspbian \<3
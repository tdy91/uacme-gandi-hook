
# uacme-gandi-hook
DNS-01 challenge hook script of uacme for gandi.net LiveDNS API

The [gandi_nsupdate.sh][gandi_nsupdate.sh] hook script included in the distribution allows DNS [gandi.net][gandi.net] users to manage [RFC8555][RFC8555] **ACMEv2** protocol **DNS-01 challenges** using gandi.net **[LiveDNS API][gandilive-dns]** and [uacme project][uacme-github] **ACMEv2** lightweight client .

## Explanations

The **ACMEv2** protocol allows a Certificate Authority ([Let's Encrypt][le] 
is a popular one) and an applicant to automate the process of verification and certificate issuance.

The **DNS-01 challenge** is a challenge type which is used to prove that you control the DNS for your domain name by putting a specific value in a TXT record under that domain name.
This challenge **must be used** to allow Let's Encrypt to issue **wildcard certificates** as specified on [Let's Encrypt Challenge Types][ls-challenge-types] documentation page.

The **uacme** [github project][uacme-github] is a lightweight client for the [RFC8555][RFC8555] ACMEv2 protocol, written in plain C with minimal dependencies; for instance, an [uacme package][openwrt-uacme-package] is available for [OpenWrt][openwrt] Linux operating system targeting embedded devices.

## Installation

Refer to [uacme manual][uacme-manual] and [README.md][uacme-github] for hook script usage with uacme.

All files of this repository have to be placed in the same folder


##### gandi_api_key - API key file

This file must contain gandi.net API key of your user account.

Refer to Gandi documentation to obtain your API key from your gandi.net account parameters, then
replace "replace-me-by-your-gandi-livedns-api-key" first line with this Gandi API key.

For security reason, make **gandi_api_key** file permission rights to "rw------- root root" using following command :
```
chmod 600 gandi_api_key
```

##### gandi_nsupdate.sh - Hook script

This hook script must be must be made executable, using following command :
```
chmod +x gandi_nsupdate.sh
```
Note : gandi_api_functions.inc file contains specific gandi.net functions; it is sourced by the hook script gandi_nsupdate.sh.

gandi_nsupdate.sh hook script is designed to be used as nsupdate.sh script described in [uacme manual][uacme-manual] and [README.md][uacme-github].

Example of uacme command line used for test purpose (--staging option, using Let's Encrypt staging URL instead of production URL) to automate Let'Encrypt certficate updates using DNS-01 challenge for site1.example.com DNS Common Name with DNS Alternative Names site2.example.com and site3.example.com.
```
uacme --staging -v -c /etc/config/uacme.d \
      -h /usr/share/uacme/gandi_nsupdate.sh \
      issue site1.example.com site2.example.com site3.example.com
```

Example of uacme command line used for production purpose to automate Let'Encrypt certficate updates using DNS-01 challenge for DNS Common Name www.your.domain.com
```
uacme -v -c /etc/config/uacme.d \
      -h /usr/share/uacme/gandi_nsupdate.sh \
      issue www.your.domain.com
```



[gandi_nsupdate.sh]: https://github.com/tdy91/uacme-gandi-hook/blob/master/gandi_nsupdate.sh
[RFC8555]: https://tools.ietf.org/html/rfc8555
[le]: https://letsencrypt.org
[gandi.net]:https://www.gandi.net
[gandilive-dns]:https://api.gandi.net/docs/livedns/
[ls-challenge-types]:https://letsencrypt.org/docs/challenge-types/
[uacme-github]:https://github.com/ndilieto/uacme
[uacme-manual]:https://ndilieto.github.io/uacme/uacme.html
[openwrt]:https://openwrt.org/
[openwrt-uacme-package]:https://github.com/openwrt/packages/tree/master/net/uacme

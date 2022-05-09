#!/bin/sh
# Copyright (C) 2021 Thierry Duvernoy <tduvernoy@free.fr>
#
# This file is part of uacme-gandi-hook.
#
# uacme-gandi-hook  is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as 
# published by the Free Software Foundation, either version 3 of 
# the License, or (at your option) any later version.
#
# uacme-gandi-hook is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

__GANDI_DNS_SERVERS="ns-202-a.gandi.net ns-132-b.gandi.net ns-44-c.gandi.net"
__ENDPOINT="https://dns.api.gandi.net/api/v5"
__RRTYPE="TXT"
__DEFAULT_TTL=600
if [ -f "$GANDI_API_KEY_FILE" ]; then
  __API_KEY_FILE=$GANDI_API_KEY_FILE
else
  __API_KEY_FILE="gandi_api_key"  
fi
__API_KEY=$(cat $__API_KEY_FILE)

# Name        : acme_ns_exists
# Description : check if fqdn exists
# Param in    : fqdn (ex : _acme-challenge.aaa...xxx.dom.com)
# Return      : 0 if exists, 1 if does not exist, 2 on error.
acme_ns_exists() {
  local fqdn=$1  # ex : fqdn = _acme-challenge.aaa...xxx.dom.com

  fqdn_woext=${fqdn%.*};ext=${fqdn##*.}
  domain=${fqdn_woext##*.}"."$ext  # ex : domain = dom.com
  ident=${fqdn_woext%.*}           # ex : ident = _acme-challenge.aaa...xxx      

  CURL_RESP=$(curl -s -X GET "$__ENDPOINT/domains/$domain/records/$ident/$__RRTYPE" \
    -H "X-Api-Key: $__API_KEY" \
    -H "Content-Type: application/json" \
    -o /dev/null \
    -w "%{http_code}")
  
  # CURL_RESP = http_code
  # 200   : fqdn exits
  # 404   : fqdn does not exist
  # other : error
  #         401 : Bad authentication attempt because of a wrong API Key.
  #         403 : Access to the resource is denied. Mainly due to a lack of permissions to access it
  
  [ "$CURL_RESP" == "200" ] && return 0 || [ "$CURL_RESP" == "404" ] && return 1 || return 2
}

# Name        : acme_ns_put
# Description : create or update acme text record.
# Param in 1  : fqdn (ex : _acme-challenge.aaa...xxx.dom.com)
# Param in 2  : challenge (auth)
# Param in 3  : ttl
# Return      : 0 on success, 1 on error
acme_ns_put() {
  local fqdn=$1  # ex : fqdn = _acme-challenge.aaa...xxx.dom.com
  local challenge="$2"
  local ttl=$3
  [ "$ttl" == "" ] && ttl=$__DEFAULT_TTL

  fqdn_woext=${fqdn%.*};ext=${fqdn##*.}
  domain=${fqdn_woext##*.}"."$ext  # ex : domain = dom.com
  ident=${fqdn_woext%.*}           # ex : ident = _acme-challenge.aaa...xxx      

  CURL_RESP=$(curl -s -X PUT "$__ENDPOINT/domains/$domain/records/$ident/$__RRTYPE" \
    -H "X-Api-Key: $__API_KEY" \
    -H "Content-Type: application/json" \
    -d "{\"rrset_ttl\": $ttl, \"rrset_values\": [\"$challenge\"]}" \
    -o /dev/null \
    -w "%{http_code}")
  
  # CURL_RESP = http_code
  # 201   : The resource has been updated
  # other : error
  #         401 : Bad authentication attempt because of a wrong API Key.
  #         403 : Access to the resource is denied. Mainly due to a lack of permissions to access it

  [ "$CURL_RESP" == "201" ] && return 0 || return 1
}

# Name        : acme_ns_delete
# Description : delete acme text record.
# Param in    : fqdn (ex : _acme-challenge.aaa...xxx.dom.com)
# Return      : 0 on success, 1 on error
acme_ns_delete() {
  local fqdn=$1  # ex : fqdn = _acme-challenge.aaa...xxx.dom.com

  fqdn_woext=${fqdn%.*};ext=${fqdn##*.}
  domain=${fqdn_woext##*.}"."$ext  # ex : domain = dom.com
  ident=${fqdn_woext%.*}           # ex : ident = _acme-challenge.aaa...xxx      

  CURL_RESP=$(curl -s -X DELETE "$__ENDPOINT/domains/$domain/records/$ident/$__RRTYPE" \
    -H "X-Api-Key: $__API_KEY" \
    -H "Content-Type: application/json" \
    -o /dev/null \
    -w "%{http_code}")
  
  # CURL_RESP = http_code
  # 204   : The resource has been deleted
  # other : error
  #         401 : Bad authentication attempt because of a wrong API Key.
  #         403 : Access to the resource is denied. Mainly due to a lack of permissions to access it
  #         404 : not found

  [ "$CURL_RESP" == "204" ] && return 0 || return 1
}

# Name        : acme_ns_post
# Description : create acme text record.
# Param in 1  : fqdn (ex : _acme-challenge.aaa...xxx.dom.com)
# Param in 2  : challenge (auth)
# Param in 3  : ttl
# Return      : 0 on success, 1 on error
acme_ns_post() {
  local fqdn=$1  # ex : fqdn = _acme-challenge.aaa...xxx.dom.com
  local challenge="$2"
  local ttl=$3
  [ "$ttl" == "" ] && ttl=$__DEFAULT_TTL

  fqdn_woext=${fqdn%.*};ext=${fqdn##*.}
  domain=${fqdn_woext##*.}"."$ext  # ex : domain = dom.com
  ident=${fqdn_woext%.*}           # ex : ident = _acme-challenge.aaa...xxx      

  CURL_RESP=$(curl -s -X POST "$__ENDPOINT/domains/$domain/records/$ident/$__RRTYPE" \
    -H "X-Api-Key: $__API_KEY" \
    -H "Content-Type: application/json" \
    -d "{\"rrset_ttl\": $ttl, \"rrset_values\": [\"$challenge\"]}" \
    -o /dev/null \
    -w "%{http_code}")
  
  # CURL_RESP = http_code
  # 200   : The resource exist with the same value (but TTL may be different)
  # 201   : Record was created
  # other : error
  #         400 : already exists with other type
  #         401 : Bad authentication attempt because of a wrong API Key.
  #         403 : Access to the resource is denied. Mainly due to a lack of permissions to access it
  #         409 : already exists (with other values)

  ([ "$CURL_RESP" == "200" ] || [ "$CURL_RESP" == "201" ]) && return 0 || return 1
}



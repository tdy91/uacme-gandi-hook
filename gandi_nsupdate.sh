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

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

# Commands
DIG=dig

# Files
# RNDC_KEY_{DIG}
#   if you wish to specify an RDC key for TSIG transactions, do so
#   here. If you do, also make sure /etc/named.conf specifies the
#   key "KEYNAME"; in the zone that must be updated (and disallow
#   all others for safety)
RNDC_KEY_DIG=

# GANDI_API_KEY_FILE
#   to be set to the path of the file containing gandi.net API key
#   if not set, gandi_api_key will be search in current scrit folder.
#   For security reason, please make gandi_api_key file permission
#   to read/write for root user and group only by issuing command :
#      chmod 600 gandi_api_key
GANDI_API_KEY_FILE="/root/gandi_api_key"
source $SCRIPTPATH/gandi_api_functions.inc

# Arguments
METHOD=$1
TYPE=$2
IDENT=$3
TOKEN=$4
AUTH=$5

ns_getdomain()
{
    local domain=$1

    [ -n "$domain" ] || return
    set -- $($DIG ${RNDC_KEY_DIG:+-k ${RNDC_KEY_DIG}} +noall +authority "$domain" SOA 2>/dev/null)

    echo $1
}

ns_getprimary()
{
    local domain=$1

    [ -n "$domain" ] || return
    set -- $($DIG ${RNDC_KEY_DIG:+-k ${RNDC_KEY_DIG}} +short "$domain" SOA 2>/dev/null)

    echo $1
}

ns_getall()
{
    local domain=$1

    [ -n "$domain" ] || return 1

    $DIG ${RNDC_KEY_DIG:+-k ${RNDC_KEY_DIG}} +short "$domain" NS 2>/dev/null
}

ns_ispresent()
{
    local fqhn="$1"
    local expect="$2"
    local domain=$(ns_getdomain "$fqhn")
    local nameservers=$(ns_getall "$domain")
    local res
    local ret

    for NS in $nameservers; do
        OLDIFS="${IFS}"
        IFS='.'
        set -- $($DIG ${RNDC_KEY_DIG:+-k ${RNDC_KEY_DIG}} +short "@$NS" "$fqhn" TXT 2>/dev/null)
        IFS="${OLDIFS}"
        { [ "$*" = "$expect" ] || [ "$*" = "\"$expect\"" ] ; } || return 1
    done

    return 0
}

gandi_ns_doupdate()
{
    local fqhn="$1"
    local challenge="$2"
    local ttl=600

    if [ -n "${challenge}" ]; then
       acme_ns_put ${fqhn} "${challenge}" ${ttl}
    else
       acme_ns_delete ${fqhn}
    fi

    return $?
}

ns_update()
{
    local fqhn="$1"
    local challenge="$2"
    local count=0
    local res

    res=1
    while [ $res -ne 0 ]; do
        if [ $count -eq 0 ]; then
            ns_doupdate "$fqhn" "$challenge"
            res=$?
            [ $res -eq 0 ] || break
        else
            sleep 1
        fi

        count=$(((count + 1) % 5))
        ns_ispresent "$fqhn" "$challenge"
        res=$?
    done

    return $res
}

ARGS=5
E_BADARGS=85

if [ $# -ne "$ARGS" ]; then
    echo "Usage: $(basename "$0") method type ident token auth" 1>&2
    exit $E_BADARGS
fi

case "$METHOD" in
    "begin")
        case "$TYPE" in
            dns-01)
                ns_update "_acme-challenge.$IDENT" "$AUTH"
                exit $?
                ;;
            *)
                exit 1
                ;;
        esac
        ;;

    "done"|"failed")
        case "$TYPE" in
            dns-01)
                ns_update "_acme-challenge.$IDENT"
                exit $?
                ;;
            *)
                exit 1
                ;;
        esac
        ;;

    *)
        echo "$0: invalid method" 1>&2
        exit 1
esac

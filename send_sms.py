# ============================================================
#  send_sms.py
#  Small helper used by the C++ program to send the SMS ticket.
#
#  The C++ program runs it like this:   python send_sms.py 9876543210
#
#  It reads 2 files from the project folder:
#     sms_key.txt  -> your Fast2SMS API key   (KEEP THIS SECRET, never upload it)
#     last_sms.txt -> the message text written by the C++ program
#
#  It ends with code 0 if the SMS was sent, or code 1 if it failed.
#  The C++ program checks this code to know what happened.
# ============================================================
import sys


def main():
    # ----- 1. Phone number from the command -----
    if len(sys.argv) < 2:
        print("  [Python] No phone number was given.")
        return 1
    phone = sys.argv[1]

    # ----- 2. The 'requests' library (used to talk to the internet) -----
    try:
        import requests
    except ImportError:
        print("  [Python] 'requests' is not installed. Run:  python -m pip install requests")
        return 1

    # ----- 3. Read the API key -----
    try:
        with open("sms_key.txt", "r") as f:
            api_key = f.read().strip()
    except FileNotFoundError:
        print("  [Python] sms_key.txt not found. Copy sms_key_example.txt to sms_key.txt")
        print("           and paste your Fast2SMS API key inside it.")
        return 1
    if api_key == "" or "PASTE" in api_key:
        print("  [Python] Your API key is not set yet. Open sms_key.txt and paste your key.")
        return 1

    # ----- 4. Read the message written by the C++ program -----
    try:
        with open("last_sms.txt", "r", encoding="utf-8") as f:
            message = f.read().strip()
    except FileNotFoundError:
        print("  [Python] last_sms.txt not found, nothing to send.")
        return 1

    # ----- 5. Send it through Fast2SMS (Quick SMS route) -----
    url = "https://www.fast2sms.com/dev/bulkV2"
    headers = {"authorization": api_key}
    payload = {"route": "q", "message": message, "numbers": phone}

    try:
        response = requests.post(url, data=payload, headers=headers, timeout=20)
    except requests.exceptions.RequestException:
        print("  [Python] Could not reach Fast2SMS. Check your internet connection.")
        return 1

    try:
        result = response.json()
    except ValueError:
        print("  [Python] Fast2SMS gave an unexpected reply:", response.text[:200])
        return 1

    # ----- 6. Check the reply -----
    if result.get("return") is True:
        print("  [Python] SMS SENT. Request ID:", result.get("request_id"))
        return 0

    print("  [Python] SMS FAILED. Fast2SMS says:", result.get("message"))
    return 1


if __name__ == "__main__":
    sys.exit(main())

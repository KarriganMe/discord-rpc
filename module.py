import threading
import time
import requests
import warnings
from urllib3.exceptions import InsecureRequestWarning
import sys

warnings.simplefilter('ignore', InsecureRequestWarning)

try:
    from pypresence import Presence
except ImportError as e:
    print(f"ERROR: pypresence not installed: {e}")
    sys.exit(1)

data = {
    "state": "Online",
    "description": "Waiting for game..."
}
cid = None  # Will be set by user via GUI
game = None
rpc = None
rpc_lock = threading.Lock()

def start_monitoring():
    thread1 = threading.Thread(target=update, daemon=True)
    thread2 = threading.Thread(target=get_data, daemon=True)
    thread1.start()
    thread2.start()

def man_data(state, description, large_image=""):
    global data
    data = {
        "state": state if state else "Online",
        "description": description if description else "Idle",
        "large_image": large_image
    }
    force_discord_update()

def send_game(games):
    global game
    game = games

def update_cid(new_cid):
    global cid, rpc
    cid = new_cid
    
    # Reconnect with new CID
    with rpc_lock:
        if rpc:
            try:
                rpc.close()
            except:
                pass
            rpc = None

def force_discord_update():
    """Force an immediate Discord update"""
    global rpc, data, cid
    
    if cid is None:
        return
    
    with rpc_lock:
        try:
            if rpc is None:
                rpc = Presence(cid)
                rpc.connect()
            
            rpc.update(
                state=data.get("state") or "Online",
                details=data.get("description") or "Idle",
                large_image=data.get("large_image") if data.get("large_image") else None,
                large_text="Playing"
            )
            
        except Exception as e:
            print(f"Discord update failed: {e}")
            rpc = None

def update():
    global data, cid, rpc
    last_state = None
    
    while True:
        try:
            if cid is None:
                time.sleep(5)
                continue
                
            with rpc_lock:
                # Try to connect if not connected
                if rpc is None:
                    try:
                        rpc = Presence(cid)
                        rpc.connect()
                    except Exception:
                        rpc = None
                        time.sleep(10)
                        continue
                
                # Update if data changed
                current_state = f"{data.get('state')}|{data.get('description')}"
                if rpc is not None and current_state != last_state:
                    try:
                        rpc.update(
                            state=data.get("state") or "Online",
                            details=data.get("description") or "Idle",
                            large_image=data.get("large_image") if data.get("large_image") else None,
                            large_text="Playing"
                        )
                        last_state = current_state
                    except Exception:
                        rpc = None
                        last_state = None
                        
        except Exception:
            rpc = None
            
        time.sleep(5)

def get_data():
    global data, game
    
    while True:
        try:
            if game == "League of Legends.exe":
                url = "https://127.0.0.1:2999/liveclientdata/allgamedata"
                try:
                    response = requests.get(url, verify=False, timeout=2)
                    response = response.json()
                    player = response['activePlayer']['summonerName']
                    player_gold = int(response['activePlayer']['currentGold'])

                    player_data = None
                    for p in response['allPlayers']:
                        if p['summonerName'] == player:
                            player_data = p
                            break
                    
                    loldata = {
                        'gold': player_gold,
                        'champion': player_data['championName'],
                        'level': player_data['level'],
                        'kills': player_data['scores']['kills'],
                        'deaths': player_data['scores']['deaths'],
                        'assists': player_data['scores']['assists'],
                        'mode': response['gameData']['gameMode']
                    }
                    
                    data = {
                        "state": f"Playing {loldata['champion']} (Lvl {loldata['level']})",
                        "description": f"{loldata['kills']}/{loldata['deaths']}/{loldata['assists']} | {loldata['gold']} gold",
                        "large_image": f"https://ddragon.leagueoflegends.com/cdn/15.24.1/img/champion/{loldata['champion']}.png"
                    }
                    
                except requests.exceptions.RequestException:
                    data = {"state": "League of Legends", "description": "In menus"}
                except Exception:
                    data = {"state": "League of Legends", "description": "Loading..."}
                
        except Exception:
            pass
            
        time.sleep(5)

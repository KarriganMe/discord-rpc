from pypresence import Presence
import time

client_id = '1410296139089051648'

print("Testing Discord RPC connection...")
print(f"Client ID: {client_id}")

try:
    RPC = Presence(client_id)
    print("Connecting...")
    RPC.connect()
    print("✓ Connected!")
    
    print("Updating presence...")
    RPC.update(state="Testing", details="RPC Test")
    print("✓ Updated!")
    
    print("Waiting 10 seconds... (check Discord)")
    time.sleep(10)
    
    RPC.close()
    print("✓ Test complete!")
    
except Exception as e:
    print(f"✗ Error: {e}")
    import traceback
    traceback.print_exc()

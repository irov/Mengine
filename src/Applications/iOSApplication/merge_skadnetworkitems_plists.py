import os
import sys
import plistlib

def merge_plists(base, patches, output_path):
    if not os.path.exists(base):
        print(f"Base plist file not found: {base}")
        sys.exit(1)
    
    with open(base, 'rb') as f:
        plist_base = plistlib.load(f)
    
    for patch in patches:
        if not os.path.exists(patch):
            print(f"Patch plist file not found: {patch}")
            sys.exit(1)
            
        with open(patch, 'rb') as f:
            plist_patch = plistlib.load(f)
        
        if isinstance(plist_patch, dict):
            if 'SKAdNetworkItems' not in plist_patch:
                print(f"Patch plist file not exist 'SKAdNetworkItems': {patch}")
                sys.exit(1)
        
            plist_base['SKAdNetworkItems'] = plist_base.get('SKAdNetworkItems', []) + [item for item in plist_patch['SKAdNetworkItems'] if item not in plist_base.get('SKAdNetworkItems', [])]
        elif isinstance(plist_patch, list):
            plist_base['SKAdNetworkItems'] = plist_base.get('SKAdNetworkItems', []) + [item for item in plist_patch if item not in plist_base.get('SKAdNetworkItems', [])]

    output_dir = os.path.dirname(output_path)
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    with open(output_path, 'wb') as f:
        plistlib.dump(plist_base, f)

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: merge_plists.py <base_plist> <patch_plists> <output_plist>")
        sys.exit(1)

    base = sys.argv[1]
    patches = sys.argv[2:-1]
    total = sys.argv[-1]

    merge_plists(base, patches, total)

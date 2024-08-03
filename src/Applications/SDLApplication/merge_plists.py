import sys
import plistlib

def merge_plists(base, patchs, output_path):
    with open(base, 'rb') as f:
        plist_base = plistlib.load(f)
    
    for patch in patches:
        with open(patch, 'rb') as f:
            plist_patch = plistlib.load(f)
        
        plist_base = {**plist_base, **plist_patch}

        if 'SKAdNetworkItems' in plist_base and 'SKAdNetworkItems' in plist_patch:
            plist_base['SKAdNetworkItems'] = plist_base['SKAdNetworkItems'] + [
                item for item in plist_patch['SKAdNetworkItems']
                if item not in plist_base['SKAdNetworkItems']
            ]

    with open(output_path, 'wb') as f:
        plistlib.dump(plist_base, f)

base = sys.argv[1]
patchs = sys.argv[2:-1]
total = sys.argv[-1]

merge_plists(base, patchs, total)
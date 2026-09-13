import plistlib
import re
import sys
import xml.etree.ElementTree as ElementTree


def convertSKAdNetworkItems(_sourcePath, _outputPath):
    with open(_sourcePath, 'rb') as source:
        data = source.read()

    if data.startswith(b'bplist00'):
        items = plistlib.loads(data)
    else:
        root = ElementTree.fromstring(data)

        if root.tag == 'plist':
            items = plistlib.loads(data)
        elif root.tag == 'SKAdNetworkItems':
            items = []

            for child in root:
                if child.tag != 'SKAdNetworkIdentifier' or len(child) != 0:
                    raise ValueError('Unexpected element in SKAdNetworkItems XML')

                items.append({'SKAdNetworkIdentifier': (child.text or '').strip()})
        else:
            raise ValueError('Expected an Apple plist or Google SKAdNetworkItems XML')

    if isinstance(items, dict):
        items = items.get('SKAdNetworkItems')

    if not isinstance(items, list) or not items:
        raise ValueError('Expected a non-empty SKAdNetworkItems array')

    uniqueItems = []
    identifiers = set()

    for item in items:
        identifier = item.get('SKAdNetworkIdentifier') if isinstance(item, dict) else None

        if not isinstance(identifier, str) or re.fullmatch(r'[a-z0-9]{10}\.skadnetwork', identifier) is None:
            raise ValueError('Invalid SKAdNetworkIdentifier: {!r}'.format(identifier))

        if identifier in identifiers:
            continue

        identifiers.add(identifier)
        uniqueItems.append(item)

    with open(_outputPath, 'wb') as output:
        plistlib.dump({'SKAdNetworkItems': uniqueItems}, output)

    print('Converted {} SKAdNetworkItems: {}'.format(len(uniqueItems), _outputPath))


if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit('Usage: convert_skadnetworkitems_plist.py <source> <output.plist>')

    try:
        convertSKAdNetworkItems(sys.argv[1], sys.argv[2])
    except (OSError, ValueError, ElementTree.ParseError) as error:
        sys.exit(str(error))

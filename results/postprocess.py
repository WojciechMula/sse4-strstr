from collections import OrderedDict

def load(file):
    D = OrderedDict()
    for line in file:
        if 'reference result' not in line:
            continue

        name, tail = line.split('...')
        name = name.strip()
        time = float(tail.split()[6])

        if name not in D:
            D[name] = time
        else:
            D[name] = min(time, D[name])

    return D


def main():
    import sys
    paths = sys.argv[1:]
    for path in paths:
        if len(paths) > 1:
            print path

        with open(path, 'rt') as f:
            for name, time in load(f).iteritems():
                print '%-30s %10.5f' % (name, time)


if __name__ == '__main__':
    main()

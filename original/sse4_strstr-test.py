import sys, os, random

filename = "<unspecified>"
try:
	filename = sys.argv[1]
	string = open(filename, "r").read()
except:
	print "can't open '%s'" % filename
	sys.exit(1)

try:
	random.seed(int(sys.argv[3]))
except:
	pass

def time_command(command):
	os.system('/usr/bin/time -o /tmp/measure -f "%U" ' + command)
	f = open("/tmp/measure", "r")
	t = float(f.read())
	f.close()
	return t


def time(command1, command2, iters=10):
	while True:
		t1 = time_command(command1.replace("__iters__", str(iters)))
		if t1 > 1:
			t2 = time_command(command2.replace("__iters__", str(iters)))
			return iters, t1, t2
		else:
			iters *= 10


def compare(filename, wordpos, word, wordlen):
	word = word.replace("%", "%%")
	cmd1 = './a.out "%s" libc __iters__ "%s" > /dev/null' % (filename, word)
	cmd2 = './a.out "%s" sse4 __iters__ "%s" > /dev/null' % (filename, word)
	_, t1, t2 = time(cmd1, cmd2)

	return "[%d,%d] libc=%0.3fs sse4=%0.3fs speedup=%0.2f" % (wordpos, wordlen, t1, t2, t1/t2)


logname   = "sse4.log"
lognumber = 1
while True:
	if not os.path.exists(logname):
		log = open(logname, "w")
		break
	else:
		logname = "sse4%d.log" % lognumber
		lognumber += 1


try:
	for n in xrange(4, 64):
		i1 = random.randint(   0, 64)
		i2 = random.randint(  65, 1024)
		i3 = random.randint(1024, len(string)-n)
		print "length", n
		for i in [i1, i2, i3]:
			word = string[i:i+n]
			for c in "\\`()<>{}\"":
				word = word.replace(c, "\\" + c)
			
			cmd = './a.out "%s" verify 1 "%s"' % (filename, word)
			err = os.system(cmd)
			if err:
				print repr(string[i:i+l])
				sys.exit(1)
			else:
				s = compare(filename, i, word, n)
				log.write(s + "\n")
				print s
except:
	import traceback
	traceback.print_exc()
	log.close()

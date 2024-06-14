import time
import sys

def main():
    print('Hello from Python!')
    print('This is the standard output')
    print('This is the standard error', file=sys.stderr)
    print('This is a test of the emergency broadcast system')
    time.sleep(5)
    print('Goodbye from Python!')

if __name__ == '__main__':
    main()

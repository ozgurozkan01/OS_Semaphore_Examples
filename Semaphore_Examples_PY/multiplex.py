import threading

def print_text(text):
    print('Hello from', text) # Second section to be written

names = ['Alice', 'Bob', 'Charlie']
threads = []

for name in names:
    thread = threading.Thread(target=print_text, args=(name,))
    print('Starting thread', name) # First section to be written
    thread.start()
    threads.append(thread)

for name, thread in zip(names, threads):
    print('Joining thread', name)  # Third section to be written
    thread.join()

print('Done!')  # Final section to be written
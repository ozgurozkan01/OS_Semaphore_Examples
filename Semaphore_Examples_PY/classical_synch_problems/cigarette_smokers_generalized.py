import threading
import time

# Integer variables to count ingredients on the table
num_tobacco = 0
num_paper = 0
num_match = 0

# Semaphores
agent_sem = threading.Semaphore(1)
tobacco = threading.Semaphore(0)
paper = threading.Semaphore(0)
match = threading.Semaphore(0)

# Mutex for mutual exclusion
mutex = threading.Lock()

def execute_pusher(main_ingredient, ingredient1, ingredient2, main_indicator, indicator1, indicator2):
    global num_tobacco, num_paper, num_match
    while True:
        main_ingredient.acquire()
        with mutex:
            if indicator1 > 0:
                indicator1 -= 1
                ingredient2.release()
            elif indicator2 > 0:
                indicator2 -= 1
                ingredient1.release()
            else:
                main_indicator += 1
        time.sleep(1)  # Simulate some delay

def execute_agent(ingredients1, ingredients2, agent_code, staff_on_table1, staff_on_table2):
    while True:
        agent_sem.acquire()
        print(f"Agent {agent_code} put {staff_on_table1}, {staff_on_table2}")
        ingredients1.release()
        ingredients2.release()
        time.sleep(1)  # Simulate some delay

def execute_smokers(main_ingredients, staff_on_table1, staff_on_table2):
    while True:
        main_ingredients.acquire()
        print(f"Smoker took {staff_on_table1} and {staff_on_table2} then made cigarette...")
        agent_sem.release()
        print("Smoking...")
        time.sleep(1)  # Simulate some delay

def run():
    # Threads for agents
    agents = [
        threading.Thread(target=execute_agent, args=(tobacco, paper, "A", "tobacco", "paper")),
        threading.Thread(target=execute_agent, args=(paper, match, "B", "paper", "match")),
        threading.Thread(target=execute_agent, args=(tobacco, match, "C", "tobacco", "match"))
    ]

    # Threads for smokers
    smokers = [
        threading.Thread(target=execute_smokers, args=(match, "tobacco", "paper")),
        threading.Thread(target=execute_smokers, args=(tobacco, "paper", "match")),
        threading.Thread(target=execute_smokers, args=(paper, "tobacco", "match"))
    ]

    # Threads for pushers
    pushers = [
        threading.Thread(target=execute_pusher, args=(tobacco, paper, match, lambda: num_tobacco, lambda: num_paper, lambda: num_match)),
        threading.Thread(target=execute_pusher, args=(paper, tobacco, match, lambda: num_paper, lambda: num_tobacco, lambda: num_match)),
        threading.Thread(target=execute_pusher, args=(match, tobacco, paper, lambda: num_match, lambda: num_tobacco, lambda: num_paper))
    ]

    # Start all threads
    for agent in agents:
        agent.start()
    for smoker in smokers:
        smoker.start()
    for pusher in pushers:
        pusher.start()

    # Wait for all threads to complete (they will not terminate in this example)
    for agent in agents:
        agent.join()
    for smoker in smokers:
        smoker.join()
    for pusher in pushers:
        pusher.join()

if __name__ == "__main__":
    run()

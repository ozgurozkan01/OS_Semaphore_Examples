import threading
import time

"""
    - Definition of the Parnas !!
    The Parnas solution is a synchronization solution for the Cigarette Smokers Problem. 
    Using semaphores and a critical region, it coordinates the interactions of multiple agents
    (the material setter) and smokers (the smoker who uses the materials to smoke a cigarette).
    The logic of the solution is to synchronize between the agents who put the ingredients on the table and the smokers who pick them up and smoke.
    
    - The Logic of the Parnas !!
    The main goal of the Parnas solution is synchronization and deadlock prevention. 
    Agents put materials on the table and smokers take these materials and smoke.
    Synchronization aims to properly coordinate an agent's placing of ingredients and a smoker's taking of those ingredients.
    It also has the goal of deadlock prevention, i.e. keeping agents and smokers working in synchronization and preventing deadlocks.

    - Steps of Parnas !!
    Agents: Each agent places a certain pair of materials (tobacco, paper, spades) on the table. 
        For example, an agent can put tobacco and paper. The agent uses the agentSem semaphore to put ingredients on the table.
        This semaphore controls the process of agents putting ingredients on the table. 
        Once the materials are placed on the table, the semaphores are released for the appropriate smoker to take the materials.
    
    Smokers: Each smoker smokes cigarettes with specific materials. The smokers wait for the respective semaphores 
        to take the materials waiting on the table. The smoker acquires the two semaphores (_ingredients1 and _ingredients2)
        needed to get the required ingredients and make a cigarette. After acquiring the ingredients,
        the smoker releases the agentSem semaphore for the agents to place the next ingredient.
    
    Pushers: Pushers are the functions responsible for putting ingredients on the table. 
        They receive semaphore signals from agents and update the materials accordingly. 
        The Pusher checks what supplies are on the table and replenishes missing supplies.
    
    Critical Zone and Synchronization: by using mutex, multiple pushers are prevented from changing the table at the same time and
        data competitions are avoided. By using semaphores (agentSem, tobacco, paper, match), material placement and retrieval are synchronized.
        
    - Advantages of the Parnas Solution !!
    Deadlock Prevention: By using semaphores and mutex, it ensures that the whole system works properly and prevents deadlocks. 
        This increases the reliability of the system.
    
    Ensuring Synchronization: It properly coordinates the interactions between agents and smokers so that all processes run in a synchronized manner.
    
    Simple and Effective: The solution provides a simple and effective synchronization using semaphores and critical regions. 
        This both makes the code easier to understand and improves performance.
        
    - CODE OUTPUT !!
        Agent A put tobacco, paper
        Smoker took paper and match then made cigarette...
        Smoking...
        Agent B put paper, match
        Smoker took tobacco and paper then made cigarette...
        Smoking...
        Agent C put tobacco, matchSmoker took tobacco and match then made cigarette...
        Smoking...
        ...
        ...
"""

# Boolean variables indicating whether an ingredient is on the table
is_tobacco = threading.Event()
is_paper = threading.Event()
is_match = threading.Event()

# Semaphores
agent_sem = threading.Semaphore(1)
tobacco = threading.Semaphore(0)
paper = threading.Semaphore(0)
match = threading.Semaphore(0)

# Mutex for mutual exclusion
mutex = threading.Lock()

def execute_pusher(main_ingredient, ingredient1, ingredient2, main_indicator, indicator1, indicator2):
    while True:
        main_ingredient.acquire()
        with mutex:
            if indicator1.is_set():
                indicator1.clear()
                ingredient2.release()
            elif indicator2.is_set():
                indicator2.clear()
                ingredient1.release()
            else:
                main_indicator.set()
        time.sleep(1)  # Added sleep to simulate some delay

def execute_agent(ingredient1, ingredient2, agent_code, staff_on_table1, staff_on_table2):
    while True:
        agent_sem.acquire()
        print(f"Agent {agent_code} put {staff_on_table1}, {staff_on_table2}")
        ingredient1.release()
        ingredient2.release()
        time.sleep(1)  # Added sleep to simulate some delay

def execute_smokers(main_ingredients, staff_on_table1, staff_on_table2):
    while True:
        main_ingredients.acquire()
        print(f"Smoker took {staff_on_table1} and {staff_on_table2} then made cigarette...")
        agent_sem.release()
        print("Smoking...")
        time.sleep(1)  # Added sleep to simulate some delay

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
        threading.Thread(target=execute_pusher, args=(tobacco, paper, match, is_tobacco, is_paper, is_match)),
        threading.Thread(target=execute_pusher, args=(paper, tobacco, match, is_paper, is_tobacco, is_match)),
        threading.Thread(target=execute_pusher, args=(match, tobacco, paper, is_match, is_tobacco, is_paper))
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

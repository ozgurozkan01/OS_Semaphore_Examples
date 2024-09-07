import threading
from threading import Semaphore
import time

"""
            There are three types of agents and three types of smokers in the code.
            Each agent puts two specific ingredients on the table. Each smoker is
            next in line to roll and smoke a cigarette after taking the two
            components he does not have. For example, if a smoker has paper, the
            other two components, tobacco and matches, must be on the table.

            - DEADLOCK !!
            At the beginning , This example has the possibility of deadlock.
            Imagine that the agent puts out tobacco and paper. Since the smoker with matches is waiting on tobacco,
            it might be unblocked. But the smoker with tobacco is waiting on paper,
            so it is possible (even likely) that it will also be unblocked.
            Then the first thread will block on paper and the second will block on match. Deadlock!

            Deadlock Example:
             Agent A puts “tobacco” and “paper” on the table. In this situation:
                * The smoker with “matches” (Smoker 3) wakes up because he is waiting for “tobacco” and “paper”.
                * At the same time, the smoker with “tobacco” (Smoker 2) wakes up expecting “paper”.
            However, both “tobacco” and “paper” smokers are waiting for “matches”, so both are blocked:
                * Smoker 2 waits for “match”.
                * Smoker 3 waits for “paper”.
            In this case, both smokers wait forever (deadlock occurs).

            - CODE OUTPUT !!
                Agent A put tobacco, paper
                Smoker took tobacco and paper, then smoking...
                Agent B put paper, match
                Smoker took paper and match, then smoking...
                Agent C put tobacco, match
                Smoker took tobacco and match, then smoking...
                Agent A put tobacco, paper
                !!DEADLOCK!!
"""

agents_amount = 3
smokers_amount = 3

agent_sem = Semaphore(1)
tobacco = Semaphore(0)
paper = Semaphore(0)
match = Semaphore(0)


def execute_agent(ingredient1, ingredient2, agent_code, put_on_table1, put_on_table2):
    while True:
        agent_sem.acquire()
        print(f"Agent {agent_code} put {put_on_table1}, {put_on_table2}")
        ingredient1.release()
        ingredient2.release()
        time.sleep(1)  # Simulate some delay


def execute_smokers(ingredient1, ingredient2, take_on_table1, take_on_table2):
    while True:
        ingredient1.acquire()
        ingredient2.acquire()
        print(f"Smoker took {take_on_table1} and {take_on_table2}, then smoking...")
        agent_sem.release()
        time.sleep(1)  # Simulate some delay


def run():
    agent_code_A = "A"
    agent_code_B = "B"
    agent_code_C = "C"

    match_staff = "match"
    tobacco_staff = "tobacco"
    paper_staff = "paper"

    agents = [
        threading.Thread(target=execute_agent, args=(tobacco, paper, agent_code_A, tobacco_staff, paper_staff)), # Agent A
        threading.Thread(target=execute_agent, args=(paper, match, agent_code_B, paper_staff, match_staff)), # Agent B
        threading.Thread(target=execute_agent, args=(tobacco, match, agent_code_C, tobacco_staff, match_staff)) # Agent C
    ]

    smokers = [
        threading.Thread(target=execute_smokers, args=(tobacco, paper, tobacco_staff, paper_staff)), # Smoker with matches
        threading.Thread(target=execute_smokers, args=(paper, match, paper_staff, match_staff)), # Smoker with tobacco
        threading.Thread(target=execute_smokers, args=(tobacco, match, tobacco_staff, match_staff)) # Smoker with paper
    ]

    for agent in agents:
        agent.start()

    for smoker in smokers:
        smoker.start()

    for agent in agents:
        agent.join()

    for smoker in smokers:
        smoker.join()


if __name__ == "__main__":
    run()

class Node:
    def __init__(self, val):
        self.data = val
        self.next = None

class SinglyLinkedList:
    def __init__(self):
        self.head = None

    def append(self, val):
        new_node = Node(val)
        if not self.head:
            self.head = new_node
        else:
            current = self.head
            while current.next:
                current = current.next
            current.next = new_node

    def search(self, val):
        current = self.head
        while current:
            if current.data == val:
                return True
            current = current.next
        return False

    def delete_value(self, val):
        if not self.head:
            return False

        if self.head.data == val:
            temp = self.head
            self.head = self.head.next
            del temp
            return True

        current = self.head
        while current.next and current.next.data != val:
            current = current.next

        if current.next:
            temp = current.next
            current.next = current.next.next
            del temp
            return True

        return False

    def display(self):
        current = self.head
        while current:
            print(f"{current.data} -> ", end="")
            current = current.next
        print("None")

    def size(self):
        count = 0
        current = self.head
        while current:
            count += 1
            current = current.next
        return count

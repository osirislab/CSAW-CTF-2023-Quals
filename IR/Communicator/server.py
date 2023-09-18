import yaml

def check_answer(question, check):
    print(question)
    while True:
        print("Please enter your answer: ", end='')
        answer = input("")
        answer_array = answer.split(',')
        if all(x in check for x in answer_array) and len(list(set(answer_array))) == len(check):
            print("That's right!")
            break
        else:
            print("Not quite... Please try again!")

if __name__ == "__main__":
    with open("config.yaml", "r") as f:
        config = yaml.safe_load(f)
    print(config['intro'])
    total_questions = len(config['questions'])
    for number, question in config["questions"].items():
        check_answer(f"({number}/{total_questions}) " + question["question"], question["answer"])
    print(config['outro'])
    print(f"Here's your flag --> {config['flag']}")

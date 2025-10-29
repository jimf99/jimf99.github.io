=== Trivia Game ==
<hr>
<pre>
import Foundation

// A dictionary to store the trivia questions and their answers.
// The key is the question (String), and the value is the answer (String).
let triviaQuestions: [String: String] = [
    "What is the capital of France?": "Paris",
    "Which planet is known as the Red Planet?": "Mars",
    "What is the largest ocean on Earth?": "Pacific",
    "Who wrote 'Romeo and Juliet'?": "Shakespeare"
]

// Variable to keep track of the player's score.
var score = 0

// Array to keep track of the questions that have already been asked.
var askedQuestions: [String] = []

func playGame() {
    print("Welcome to the Trivia Game!")
    print("Let's test your knowledge.")

    // Play until all questions are asked.
    while askedQuestions.count < triviaQuestions.count {
        // Find a question that hasn't been asked yet.
        guard let question = triviaQuestions.keys.randomElement(),
              !askedQuestions.contains(question) else {
            continue
        }

        print("\nQuestion: \(question)")
        
        // Get the correct answer from the dictionary.
        guard let correctAnswer = triviaQuestions[question] else {
            continue
        }

        // Add the question to the list of asked questions.
        askedQuestions.append(question)
        
        print("Your answer: ", terminator: "")
        let userAnswer = readLine() ?? "\(correctAnswer)"
        print(userAnswer)

        
        // Check if the user's answer is correct (case-insensitive).
        if userAnswer.lowercased() == correctAnswer.lowercased() {
            print("That's right!")
            score += 1
        } else {
            print("Incorrect. The correct answer was: \(correctAnswer)")
        }
    }
    
    print("\nGame over! You've answered all the questions.")
    print("Your final score is: \(score) out of \(triviaQuestions.count)")
}

// Start the game.
playGame()

  
</pre>

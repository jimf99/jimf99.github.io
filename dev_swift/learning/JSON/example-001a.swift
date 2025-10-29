import Foundation

// A struct representing a `User` that conforms to `Codable`.
struct User: Codable {
  var id: Int
  var name: String
  var email: String
  var isActive: Bool
}

// Function to get the file URL for our JSON file.
func getFileURL() -> URL {
  let fileManager = FileManager.default
  let currentDirectoryURL = URL(fileURLWithPath: fileManager.currentDirectoryPath)
  return currentDirectoryURL.appendingPathComponent("users.json")
}

// Function to encode and write JSON data to a file.
func writeJSONFile(users: [User]) {
  do {
    // 1. Create a JSONEncoder instance.
    let encoder = JSONEncoder()
    // 2. Format the output for readability.
    encoder.outputFormatting = .prettyPrinted

    // 3. Encode the array of `User` objects into `Data`.
    let jsonData = try encoder.encode(users)

    // 4. Write the encoded data to the file URL.
    try jsonData.write(to: getFileURL(), options: .atomic)
    print("Successfully wrote JSON to users.json")

  } catch {
    print("Error writing JSON file: \(error.localizedDescription)")
  }
}

// Function to read and decode JSON data from a file.
func readJSONFile() -> [User]? {
  do {
    // 1. Read the raw `Data` from the file URL.
    let jsonData = try Data(contentsOf: getFileURL())

    // 2. Create a JSONDecoder instance.
    let decoder = JSONDecoder()

    // 3. Decode the JSON `Data` back into an array of `User` objects.
    let users = try decoder.decode([User].self, from: jsonData)
    print("Successfully read and decoded JSON data.")
    return users

  } catch {
    print("Error reading or decoding JSON file: \(error.localizedDescription)")
    return nil
  }
}

// --- Main execution ---

// 1. Create some sample data.
let sampleUsers = [
  User(id: 1, name: "Alice", email: "alice@example.com", isActive: true),
  User(id: 2, name: "Bob", email: "bob@example.com", isActive: false),
  User(id: 3, name: "Charlie", email: "charlie@example.com", isActive: true),
]

// 2. Write the sample data to a JSON file.
writeJSONFile(users: sampleUsers)

// 3. Read the data back from the JSON file.
if let loadedUsers = readJSONFile() {
  print("\n--- Loaded Users ---")
  for user in loadedUsers {
    print("ID: \(user.id), Name: \(user.name), Email: \(user.email)")
  }
}



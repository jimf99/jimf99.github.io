import Foundation

let fileURL = URL(fileURLWithPath: "config.plist")

// Check if the plist file exists
func fileExists() -> Bool {
    return FileManager.default.fileExists(atPath: fileURL.path)
}

// Write data to the plist file
func writePlist(data: [String: Any]) {
    do {
        // Create the plist file if it doesn't exist
        if !fileExists() {
            FileManager.default.createFile(atPath: fileURL.path, contents: nil)
        }

        // Serialize and write the data
        let plistData = try PropertyListSerialization.data(fromPropertyList: data, format: .xml, options: 0)
        try plistData.write(to: fileURL)
        print("Wrote plist to \(fileURL.path)")
    } catch {
        print("Error writing plist: \(error.localizedDescription)")
    }
}

// Read data from the plist file
func readPlist() -> [String: [String: Any]]? {
    do {
        guard fileExists() else {
            print("Error: File does not exist.")
            return nil
        }

        let data = try Data(contentsOf: fileURL)
        if let plist = try PropertyListSerialization.propertyList(from: data, options: [], format: nil) as? [String: [String: Any]] {
            print("Read plist:", plist)
            return plist
        }
    } catch {
        print("Error reading plist: \(error.localizedDescription)")
    }
    return nil
}

// Add a new user
func addUser(username: String, details: [String: Any]) {
    print("")
    var currentData = readPlist() ?? [:]
    currentData[username] = details
    writePlist(data: currentData)
}

// Get a specific user
func getUser(username: String) -> [String: Any]? {
    let currentData = readPlist() ?? [:]
    return currentData[username]
}

// Update user details
func updateUser(username: String, details: [String: Any]) {
    var currentData = readPlist() ?? [:]
    if currentData.keys.contains(username) {
        currentData[username]?.merge(details) { (current, _) in current } // Merge existing data
        writePlist(data: currentData)
    } else {
        print("User '\(username)' does not exist.")
    }
}

// Delete a user
func deleteUser(username: String) {
    var currentData = readPlist() ?? [:]
    if currentData.keys.contains(username) {
        currentData.removeValue(forKey: username)
        writePlist(data: currentData)
    } else {
        print("User '\(username)' does not exist.")
    }
}

// Read file contents as ASCII
func readFileAsASCII() {
    print("")
    do {
        let data = try Data(contentsOf: fileURL)
        if let asciiString = String(data: data, encoding: .ascii) {
            print("Contents of the plist as ASCII:\n\(asciiString)")
        } else {
            print("Error: Could not convert data to ASCII.")
        }
    } catch {
        print("Error reading file as ASCII: \(error.localizedDescription)")
    }
}

// Example Usage
let user1: [String: Any] = [
    "retryCount": 3,
    "enabled": true
]

let user2: [String: Any] = [
    "retryCount": 5,
    "enabled": false
]

print("")
print("=== BEGIN ===")
print("")

// Adding users
addUser(username: "userName1", details: user1)
addUser(username: "userName2", details: user2)

// Getting a specific user
if let userDetails = getUser(username: "userName1") {
    print("Details for userName1: \(userDetails)")
} else {
    print("User 'userName1' does not exist.")
}

// Updating user1's details
updateUser(username: "userName1", details: ["retryCount": 4])

// Deleting user2
deleteUser(username: "userName2")


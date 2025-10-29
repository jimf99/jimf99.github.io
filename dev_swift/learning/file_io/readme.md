=== File IO ===
<hr>
```swiftlang
import Foundation

// Define the file URL in the current working directory.
let fileURL = URL(fileURLWithPath: "test.txt")
let content = "Hello from the command line!"

do {
    try content.write(to: fileURL, atomically: true, encoding: .utf8)
    let readContent = try String(contentsOf: fileURL)
    print("Read content: \(readContent)")
} catch {
    print("Error during file operation: \(error)")
}

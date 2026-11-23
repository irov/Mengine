import Foundation
import MengineSwiftLog

private func swiftLogCategory(_ file: String) -> String {
    let category = file.prefix(while: { $0 != "/" })

    return String(category)
}

public func swiftLogError(_ message: @autoclosure () -> String, file: String = #fileID,
                          line: Int32 = #line, function: String = #function) {
    MengineSwiftLogWrite(.error, { swiftLogCategory(file) }, file, line, function, message)
}

public func swiftLogWarning(_ message: @autoclosure () -> String, file: String = #fileID,
                            line: Int32 = #line, function: String = #function) {
    MengineSwiftLogWrite(.warning, { swiftLogCategory(file) }, file, line, function, message)
}

public func swiftLogMessage(_ message: @autoclosure () -> String, file: String = #fileID,
                            line: Int32 = #line, function: String = #function) {
    MengineSwiftLogWrite(.message, { swiftLogCategory(file) }, file, line, function, message)
}

public func swiftLogInfo(_ category: @autoclosure () -> String, _ message: @autoclosure () -> String,
                         file: String = #fileID, line: Int32 = #line, function: String = #function) {
    MengineSwiftLogWrite(.info, category, file, line, function, message)
}

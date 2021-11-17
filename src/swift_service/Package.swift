// swift-tools-version:5.2

import PackageDescription

let package = Package(
	name: "swift_service",
	dependencies:
	[	.package(url: "https://github.com/apple/swift-nio.git", from: "2.14.0"),
	],
	targets:
	[	.target(
			name: "swift_service",
			dependencies:
			[	.product(name: "NIO", package: "swift-nio"),
			]
		),
	]
)

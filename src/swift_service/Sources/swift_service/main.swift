import NIOCore
import NIOPosix

private final class EchoHandler: ChannelInboundHandler
{	public typealias InboundIn = ByteBuffer;
	public typealias OutboundOut = ByteBuffer;

	// New client connected
	public func channelActive(context: ChannelHandlerContext)
	{	print("Conn");
		self.writeString(context: context, data: "Hi, i'm swift_service\n");
	}

	// Read some data
	public func channelRead(context: ChannelHandlerContext, data: NIOAny)
	{	context.write(data, promise: nil);
	}

	// Ceased to read data for now
	public func channelReadComplete(context: ChannelHandlerContext)
	{	context.flush();
	}

	// Error
	public func errorCaught(context: ChannelHandlerContext, error: Error)
	{	print("error: ", error);
		context.close(promise: nil);
	}

	// I want to use this helper function to send String data
	private func writeString(context: ChannelHandlerContext, data: String)
	{	var buff = context.channel.allocator.buffer(capacity: data.count);
        buff.writeString(data);
		context.writeAndFlush(self.wrapOutboundOut(buff), promise: nil);
	}
}

let group = MultiThreadedEventLoopGroup(numberOfThreads: System.coreCount);
defer
{	try! group.syncShutdownGracefully();
}
let channel = try ServerBootstrap(group: group)
	.serverChannelOption(ChannelOptions.backlog, value: 256)
	.childChannelInitializer
	{	channel in
		// Ensure we don't read faster than we can write by adding the BackPressureHandler into the pipeline.
		channel.pipeline.addHandler(BackPressureHandler()).flatMap
		{	v in
			channel.pipeline.addHandler(EchoHandler());
		};
	}
	.childChannelOption(ChannelOptions.maxMessagesPerRead, value: 16)
	.childChannelOption(ChannelOptions.recvAllocator, value: AdaptiveRecvByteBufferAllocator())
	.bind(host: "swift_service", port: 15880)
	.wait();

print("Service started on \(channel.localAddress!)");

// This will never unblock as we don't close the ServerChannel
try channel.closeFuture.wait();

print("Service terminated");

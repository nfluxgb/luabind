function init(id)
	mylib.print_message("in init. got: " .. id)
	local announce_id = mylib.announce("henry")
	mylib.print_message("announce got " .. announce_id)
	print(mylib.get_object())
	print(mylib.get_all_objects())
	print(mylib.get_all_numbers())
end

function shutdown()
	mylib.print_message ("in shutdown")
end

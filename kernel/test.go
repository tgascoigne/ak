package kernel

//export do_something_go
func DoSomething(x int) int {
	return x * 20
}

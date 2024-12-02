package main

import (
	"io"
	"net/http"
	"strings"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

// TODO:(joao)
// 1. Resize result appropriately and show it in a color that lets you actually read it
// 2. Resize entire window appropriately and make it remember last size set

func check_err(err, msg string, callback func()) {
	// TODO:(joao) Implement this later
}

type Header struct {
	name, value *widget.Entry
	del_btn     *widget.Button
	pos         int
}

func Header_del_fn(box *fyne.Container, h *[]Header, pos int) {
	box.Objects = box.Objects[:pos]
	if pos == len(*h)-1 {
		*h = (*h)[:pos]
	} else {
		*h = append((*h)[:pos], (*h)[pos+1:]...)
	}

	for i := pos; i < len(*h); i++ {
		(*h)[i].pos = i
		(*h)[i].del_btn = widget.NewButton("X", func() {
			Header_del_fn(box, h, i)
		})
		new_header_head_val := container.NewHSplit((*h)[i].name, (*h)[i].value)
		new_header_box := container.NewHSplit(new_header_head_val, (*h)[i].del_btn)
		new_header_box.SetOffset(1.0)
		box.Objects = append(box.Objects, new_header_box)
	}

	box.Refresh()
}

func request(m, url, body string, headers *[]Header) []byte {
	client := &http.Client{}
	var method string

	switch m {
	case "GET":
		method = http.MethodGet
	case "POST":
		method = http.MethodPost
	case "PUT":
		method = http.MethodPut
	case "PATCH":
		method = http.MethodPatch
	case "DELETE":
		method = http.MethodDelete
	}

	req, err := http.NewRequest(method, url, strings.NewReader(body))
	if err != nil {
		panic(err)
	}

	if len(*headers) > 0 {
		for _, h := range *headers {
			req.Header.Add(h.name.Text, h.value.Text)
		}
	}

	res, err := client.Do(req)
	if err != nil {
		panic(err)
	}

	res_body, err := io.ReadAll(res.Body)

	if err != nil {
		panic(err)
	}

	return res_body
}

func main() {
	// Setting up application and window
	a := app.New()
	w := a.NewWindow("vcurl")
	w.Resize(fyne.NewSize(800, 400))

	// Declaring UI items
	http_method_label := widget.NewLabel("Method")
	http_method_dropdown := widget.NewSelect([]string{"GET", "POST", "PUT", "PATCH", "DELETE"}, func(value string) {
	})
	http_method_dropdown.SetSelected("GET")

	url_label := widget.NewLabel("URL")
	url_textbox := widget.NewEntry()

	body_label := widget.NewLabel("Body")
	body_textarea := widget.NewMultiLineEntry()

	result_box := widget.NewMultiLineEntry()
	result_box.Disable()

	// Headers dynamic add/remove logic
	var headers []Header
	headers_label := widget.NewLabel("Headers")
	var all_headers_box *fyne.Container

	all_headers_box = container.NewVBox()

	add_header_btn := widget.NewButton("Add Header", func() {
		new_header_pos := len(all_headers_box.Objects)
		new_header := Header{widget.NewEntry(), widget.NewEntry(), widget.NewButton("X", func() {
			Header_del_fn(all_headers_box, &headers, new_header_pos)
		}), len(all_headers_box.Objects)}
		headers = append(headers, new_header)

		new_header_head_val := container.NewHSplit(headers[new_header.pos].name, headers[new_header.pos].value)

		new_header_box := container.NewHSplit(new_header_head_val, headers[new_header.pos].del_btn)
		new_header_box.SetOffset(1.0)

		all_headers_box.Add(new_header_box)
	})

	// Submit request button logic
	submit_btn := widget.NewButton("Submit", func() {
		result_box.Text = "Requesting..."
		output := request(http_method_dropdown.Selected, url_textbox.Text, body_textarea.Text, &headers)
		result_box.Text = string(output)
		result_box.Refresh()
	})

	// Form container
	form_box := container.NewVBox(http_method_label, http_method_dropdown, url_label, url_textbox, body_label, body_textarea, headers_label, add_header_btn, all_headers_box, submit_btn)

	// And finally, running window
	w.SetContent(container.NewHSplit(form_box, result_box))
	w.ShowAndRun()
}

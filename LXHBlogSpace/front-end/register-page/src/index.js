import React from "react"
import ReactDOM from "react-dom"

import  Navbar from "./components/Navbar"
import RegiserContainer from "./components/RegisterContainer"
import Footer from "./components/Footer"

ReactDOM.render(<Navbar />, document.getElementById("navbar"));
ReactDOM.render(<RegiserContainer />, document.getElementById("register-area"));
ReactDOM.render(<Footer />, document.getElementById("footer"));
import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import FunctionButtons from "./components/FunctionButtons"

import * as serviceWorker from './serviceWorker';

function SitckyFuntionButtons()
{
    var container = document.getElementsByClassName("blog-container")[0];
    var panelLeft = container.offsetLeft + container.offsetWidth + 20;
    var panel = document.getElementById("function-buttons");
    panel.style.position = "fixed"
    panel.style.top = "80px";
    panel.style.left= panelLeft + "px";
}

ReactDOM.render(<App />, document.getElementById('app-main'));
ReactDOM.render(<FunctionButtons />, document.getElementById('function-buttons'));

window.onload = SitckyFuntionButtons;
window.onresize = SitckyFuntionButtons;
// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();

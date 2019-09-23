import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import * as serviceWorker from './serviceWorker';

import Navbar from "./components/Navbar"
import BlogEdit from "./components/BlogEdit"

ReactDOM.render(<Navbar />, document.getElementById('navbar'));
ReactDOM.render(<BlogEdit />, document.getElementById('article-edit-region'));
serviceWorker.unregister();

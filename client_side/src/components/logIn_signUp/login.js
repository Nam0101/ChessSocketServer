import '../../styles/login_signup/login.scss';
import { NavLink } from 'react-router-dom';
import { FaFacebookF } from "react-icons/fa";
import { FaGoogle } from "react-icons/fa";
import { FaTwitter } from "react-icons/fa";
import { FaGithub } from "react-icons/fa";
import { FaUser } from "react-icons/fa";
import { FaLock } from "react-icons/fa";
import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { useSelector } from 'react-redux';
//socket
const protobuf = require("protobufjs");
const net = require("net");
// server is run on port 12345
const SERVER_IP = "127.0.0.1";
const PORT = 12345;
const User = [];
const io = require("socket.io-client");
const socket = io("http://localhost:12345");
socket.on("connect", () => {
    console.log("connected");
});
socket.on("disconnect", () => {
    console.log("disconnected");
}
);
const Login = () => {
    const user = useSelector(state => state.Signup.user);
    const navigate = useNavigate();
    const [username, setUsername] = useState('');
    const [password, setPassword] = useState('');

    const clickLogin = () => {
        console.log(user);
        if (username === '' || password === '') {
            // toast.error("Please enter full information");
            return;
        }
        
        if (User && User.length === 0) {
            // toast.error("Email is incorrect");
            return;
        } else {
            if (User && User.password === password) {
                // toast.success("Signin successfully");
                navigate('/chess');
            } else {
                // toast.error("Password is incorrect");
                return;
            }
        }
    }
    return (
        <div className="loginContainer container-fluid">
            <div className="login_container">
                <div className='form_login'>
                    <div className='text-center text_login'>
                        Login
                    </div>
                    <div className="input_login form-outline mb-4">
                        <FaUser className='iconlogin' />
                        <input type="text" id="form2Example1" className='input1' required onChange={(e) => setUsername(e.target.value)} />
                        <label className="form-label label_login" htmlFor="form2Example1">User name</label>
                    </div>

                    <div className="input_login form-outline mb-4">
                        <FaLock className='iconlogin' />
                        <input type="password" id="form2Example2" className='input1' required onChange={(e) => setPassword(e.target.value)} />
                        <label className="form-label label_login" htmlFor="form2Example2">Password</label>
                    </div>

                    <div className="row mb-4">
                        <div className="col d-flex justify-content-center">
                            <div className="form-check">
                                <input className="form-check-input" type="checkbox" value="" id="form2Example31" />
                                <label className="form-check-label" htmlFor="form2Example31"> Remember me </label>
                            </div>
                        </div>

                        <div className="col">
                            <a href="#!" >Forgot password?</a>
                        </div>
                    </div>

                    <button type="button" className="btn btn-primary btn-block mb-4 w-100 " onClick={() => clickLogin()}>Sign in</button>

                    <div className="text-center">
                        <p>Not a member? <NavLink to="/signup">SRegister</NavLink></p>
                        <p>or sign up with:</p>
                        <button type="button" className="btn btn-link btn-floating mx-1">
                            <FaFacebookF className='text-white fs-4' />
                        </button>

                        <button type="button" className="btn btn-link btn-floating mx-1">
                            <FaGoogle className='text-white fs-4' />
                        </button>

                        <button type="button" className="btn btn-link btn-floating mx-1">
                            <FaTwitter className='text-white fs-4' />
                        </button>

                        <button type="button" className="btn btn-link btn-floating mx-1">
                            <FaGithub className='text-white fs-4' />
                        </button>
                    </div>
                </div>
            </div>
        </div>
    )
}

export default Login;
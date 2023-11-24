import '../../styles/login_signup/signup.scss';
import { FaFacebookF } from "react-icons/fa";
import { FaGoogle } from "react-icons/fa";
import { FaTwitter } from "react-icons/fa";
import { FaGithub } from "react-icons/fa";
import { useState } from 'react';
import { useDispatch, useSelector } from 'react-redux'
import { AddUser } from '../../store/actions/Signup';
import { useNavigate } from 'react-router-dom';
// import { toast } from 'react-toastify';
const Signup = () => {
    const dispatch = useDispatch();
    const user = useSelector(state => state.Signup.user);
    const navigate = useNavigate();
    const [name, setName] = useState('');
    const [age, setAge] = useState();
    const [gender, setGender] = useState('');
    const [username, setUsername] = useState('');
    const [password, setPassword] = useState('');

    const clickSignup = () => {
        if (name === "" || age === "" || gender === "" || username === "" || password === "") {
            // toast.error("Please enter all information");
            return;
        }
        let User = { id: user.length + 1, name: name, age: age, gender: gender, username: username, password: password }
        dispatch(AddUser(User));
        navigate('/login');
        // toast.success("Sign up successfully");
    }
    return (
        <div className="signupContainer container-fluid">
            <div className='form_signup'>
                <div className='signup_item'>
                    <input type='text' className='input_signup' required onChange={(e) => setName(e.target.value)} />
                    <label className='label_signup'>Name</label>
                </div>
                <div className='signup_item'>
                    <input type="number" id="age" name="age" min="0" max="120" className='input_signup' required onChange={(e) => setAge(e.target.value)} />
                    <label className='label_signup'>Age</label>
                </div>
                <div className='gender_item'>
                    <label className='form-check-label label_gender' id='gender_title'>Gender</label>
                    <input type="radio" name="gender" value="female" className='form-check-input' onChange={(e) => setGender(e.target.value)} />
                    <label className='form-check-label label_gender'>Female</label>
                    <input type="radio" name="gender" value="male" className='form-check-input' onChange={(e) => setGender(e.target.value)} />
                    <label className='form-check-label label_gender'>Male</label>
                    <input type="radio" name="gender" value="other" className='form-check-input' onChange={(e) => setGender(e.target.value)} />
                    <label className='form-check-label label_gender'>Other</label>
                </div>
                <div className='signup_item'>
                    <input type='text' className='input_signup' required onChange={(e) => setUsername(e.target.value)} />
                    <label className='label_signup'>User name</label>
                </div>
                <div className='signup_item'>
                    <input type='Password' className='input_signup' required onChange={(e) => setPassword(e.target.value)} />
                    <label className='label_signup'>Password</label>
                </div>
                <div>
                    <button type='submit' className='w-100 mt-5 button_submit' onClick={() => clickSignup()}>Register</button>
                </div>
                <div className="text-center ">
                    <p className='text_register'>or Register with:</p>
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
    )
}

export default Signup;
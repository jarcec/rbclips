# Copyright 2013 Jarek Jarcec Cecho <jarcec@apache.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
require 'test/unit'
require './rbclips'

class Test_Environment < Test::Unit::TestCase

  def teardown
    Clips::Base.reset
  end

  def test_exists
    assert Clips.constants.member?(:Environment)
  end

  def test_internal_buffer
    num = Clips::Environment.all.size
    Clips::Environment.new
    Clips::Environment.new
    Clips::Environment.new
    assert_equal num + 3, Clips::Environment.all.size
  end

  def test_current
    # By default there should be one default environment
    assert_not_nil Clips::Environment.current

    # Setting default environment
    a = Clips::Environment.new
    assert_not_equal a, Clips::Environment.current
    a.set_current
    assert_equal a, Clips::Environment.current
  end

  def test_equals
    a = Clips::Environment.new
    b = a.dup

    assert a == b
    assert b.eql? a
    assert b.equal? a

    a.set_current
    assert b == Clips::Environment.current
  end

  def test_cloning
    num = Clips::Environment.all.size
    a = Clips::Environment.current.clone
    b = a.dup
    c = b.clone

    assert_equal num, Clips::Environment.all.size

    assert_not_equal Clips::Environment.new, 5
  end

  def test_valid
    assert Clips::Environment.current.valid?
    assert Clips::Environment.new.valid?
  end

  def test_destroy
    num = Clips::Environment.all.size

    a = Clips::Environment.current
    assert !a.destroy!
    assert_equal num, Clips::Environment.all.size

    a = Clips::Environment.new
    assert a.valid?
    assert a.destroy!
    assert !a.valid?
    assert !a.destroy!
    assert_equal num, Clips::Environment.all.size
  end
end

